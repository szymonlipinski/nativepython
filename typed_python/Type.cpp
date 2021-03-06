/******************************************************************************
   Copyright 2017-2019 Nativepython Authors

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
******************************************************************************/

#include "AllTypes.hpp"

void Type::repr(instance_ptr self, ReprAccumulator& out) {
    assertForwardsResolved();

    this->check([&](auto& subtype) {
        subtype.repr(self, out);
    });
}

bool Type::cmp(instance_ptr left, instance_ptr right, int pyComparisonOp) {
    assertForwardsResolved();

    return this->check([&](auto& subtype) {
        return subtype.cmp(left, right, pyComparisonOp);
    });
}

int32_t Type::hash32(instance_ptr left) {
    assertForwardsResolved();

    return this->check([&](auto& subtype) {
        return subtype.hash32(left);
    });
}

void Type::move(instance_ptr dest, instance_ptr src) {
    //right now, this is legal because we have no self references.
    swap(dest, src);
}

void Type::swap(instance_ptr left, instance_ptr right) {
    assertForwardsResolved();

    if (left == right) {
        return;
    }

    size_t remaining = m_size;
    while (remaining >= 8) {
        int64_t temp = *(int64_t*)left;
        *(int64_t*)left = *(int64_t*)right;
        *(int64_t*)right = temp;

        remaining -= 8;
        left += 8;
        right += 8;
    }

    while (remaining > 0) {
        int8_t temp = *(int8_t*)left;
        *(int8_t*)left = *(int8_t*)right;
        *(int8_t*)right = temp;

        remaining -= 1;
        left += 1;
        right += 1;
    }
}

// static
char Type::byteCompare(uint8_t* l, uint8_t* r, size_t count) {
    while (count >= 8 && *(uint64_t*)l == *(uint64_t*)r) {
        l += 8;
        r += 8;
        count -= 8;
    }

    for (long k = 0; k < count; k++) {
        if (l[k] < r[k]) {
            return -1;
        }
        if (l[k] > r[k]) {
            return 1;
        }
    }
    return 0;
}

void Type::constructor(instance_ptr self) {
    assertForwardsResolved();

    this->check([&](auto& subtype) { subtype.constructor(self); } );
}

void Type::destroy(instance_ptr self) {
    assertForwardsResolved();

    this->check([&](auto& subtype) { subtype.destroy(self); } );
}

void Type::copy_constructor(instance_ptr self, instance_ptr other) {
    assertForwardsResolved();

    this->check([&](auto& subtype) { subtype.copy_constructor(self, other); } );
}

void Type::assign(instance_ptr self, instance_ptr other) {
    assertForwardsResolved();

    this->check([&](auto& subtype) { subtype.assign(self, other); } );
}

bool Type::isBinaryCompatibleWith(Type* other) {
    if (other == this) {
        return true;
    }

    while (other->getTypeCategory() == TypeCategory::catPythonSubclass) {
        other = other->getBaseType();
    }

    auto it = mIsBinaryCompatible.find(other);
    if (it != mIsBinaryCompatible.end()) {
        return it->second != BinaryCompatibilityCategory::Incompatible;
    }

    //mark that we are recursing through this datastructure. we don't want to
    //loop indefinitely.
    mIsBinaryCompatible[other] = BinaryCompatibilityCategory::Checking;

    bool isCompatible = this->check([&](auto& subtype) {
        return subtype.isBinaryCompatibleWithConcrete(other);
    });

    mIsBinaryCompatible[other] = isCompatible ?
        BinaryCompatibilityCategory::Compatible :
        BinaryCompatibilityCategory::Incompatible
        ;

    return isCompatible;
}

void Type::endOfConstructorInitialization() {
    visitReferencedTypes([&](Type* &t) {
        while (t->getTypeCategory() == TypeCategory::catForward && ((Forward*)t)->getTarget()) {
            t = ((Forward*)t)->getTarget();
        }

        if (t == this) {
            return;
        }

        if (t->getTypeCategory() == TypeCategory::catForward) {
            m_referenced_forwards.insert((Forward*)t);
            ((Forward*)t)->markIndirectForwardUse(this);
        } else {
            for (auto referencedT: t->getReferencedForwards()) {
                m_referenced_forwards.insert(referencedT);
                referencedT->markIndirectForwardUse(this);
            }
        }
    });

    visitContainedTypes([&](Type* t) {
        if (t->getTypeCategory() == TypeCategory::catForward) {
            m_contained_forwards.insert((Forward*)t);
        } else {
            for (auto containedT: t->getContainedForwards()) {
                m_contained_forwards.insert(containedT);
            }
        }
    });

    if (!m_referenced_forwards.size()) {
        forwardTypesAreResolved();
    } else {
        this->check([&](auto& subtype) {
            subtype._updateAfterForwardTypesChanged();
        });
    }
}

void Type::forwardTypesAreResolved() {
    m_resolved = true;

    this->check([&](auto& subtype) {
        subtype._updateAfterForwardTypesChanged();
    });

    if (m_is_simple) {
        bool isSimple = true;

        visitReferencedTypes([&](Type* t) { if (!t->m_is_simple) isSimple = false; });

        if (!isSimple) {
            std::function<void (Type*)> markNotSimple([&](Type* t) {
                if (t->m_is_simple) {
                    t->m_is_simple = false;
                    markNotSimple(t);
                }
            });

            markNotSimple(this);
        }
    }

    if (mTypeRep) {
        updateTypeRepForType(this, mTypeRep);
    }
}

void Type::forwardResolvedTo(Forward* forward, Type* resolvedTo) {
    if (resolvedTo->getTypeCategory() == TypeCategory::catForward) {
        throw std::runtime_error("Forwards must not resolve to other forwards.");
    }

    // make sure we reference this forward properly
    if (m_referenced_forwards.find(forward) == m_referenced_forwards.end()) {
        throw std::runtime_error(
            "Internal error: we are supposed to reference forward " +
            forward->name() + " but we don't have it marked."
        );
    }

    // swap out the type representation. we shouldn't reference this forward any more
    visitReferencedTypes([&](Type* &subtype) {
        if (subtype == forward) {
            subtype = resolvedTo;
        }
    });

    // update the 'containment' forward graph
    if (m_contained_forwards.find(forward) != m_contained_forwards.end()) {
        m_contained_forwards.erase(forward);
        for (auto contained: resolvedTo->getContainedForwards()) {
            if (contained != forward) {
                m_contained_forwards.insert(contained);
            }
        }
    }

    m_referenced_forwards.erase(forward);

    for (auto referenced: resolvedTo->getReferencedForwards()) {
        if (referenced != forward) {
            referenced->markIndirectForwardUse(this);
            m_referenced_forwards.insert(referenced);
        }
    }

    if (m_referenced_forwards.size() == 0) {
        forwardTypesAreResolved();
    }
}


PyObject* getOrSetTypeResolver(PyObject* module, PyObject* args) {
    int num_args = 0;
    if (args)
        num_args = PyTuple_Size(args);
    if (num_args > 1) {
        PyErr_SetString(PyExc_TypeError, "getOrSetTypeResolver takes 0 or 1 positional argument");
        return NULL;
    }
    static PyObject* curResolver = nullptr;
    assertHoldingTheGil();
    if (num_args == 0) {
        return curResolver;
    }

    PyObjectHolder resolver(PyTuple_GetItem(args, 0));
    //std::cerr<<" " << Py_TYPE(module)->tp_name << " " << Py_TYPE(resolver)->tp_name <<std::endl;

    decref(curResolver);
    incref(resolver);
    curResolver = resolver;
    return curResolver;
}
