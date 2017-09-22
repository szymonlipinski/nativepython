#   Copyright 2017 Braxton Mckee
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

import llvmlite.binding as llvm
import llvmlite.ir
import native_ast_to_llvm as native_ast_to_llvm
import ctypes

llvm.initialize()
llvm.initialize_native_target()
llvm.initialize_native_asmprinter()  # yes, even this one

target_triple = llvm.get_process_triple()
target = llvm.Target.from_triple(target_triple)
target_machine = target.create_target_machine()

pointer_size = (
    llvmlite.ir.PointerType(llvmlite.ir.DoubleType())
        .get_abi_size(target_machine.target_data)
    )

def create_execution_engine():
    """
    Create an ExecutionEngine suitable for JIT code generation on
    the host CPU.  The engine is reusable for an arbitrary number of
    modules.
    """
    # Create a target machine representing the host

    pmb = llvm.create_pass_manager_builder()
    pmb.opt_level = 3

    pass_manager = llvm.create_module_pass_manager()
    pmb.populate(pass_manager)

    target_machine.add_analysis_passes(pass_manager)

    # And an execution engine with an empty backing module
    backing_mod = llvm.parse_assembly("")
    engine = llvm.create_mcjit_compiler(backing_mod, target_machine)

    return engine, pass_manager

def native_to_ctype(native_type):
    if native_type.matches.Int:
        if native_type.bits == 64:
            return ctypes.c_long if native_type.signed else ctypes.c_ulong
        if native_type.bits == 32:
            return ctypes.c_int if native_type.signed else ctypes.c_uint
        if native_type.bits == 16:
            return ctypes.c_short if native_type.signed else ctypes.c_ushort
        if native_type.bits == 8:
            return ctypes.c_char if native_type.signed else ctypes.c_uchar
        if native_type.bits == 1:
            return ctypes.c_bool
    if native_type.matches.Float:        
        if native_type.bits == 64:
            return ctypes.c_double       
        if native_type.bits == 32:
            return ctypes.c_float

    if native_type.matches.Pointer:
        return ctypes.c_void_p
    
    if native_type.matches.Void:
        return None
    
    assert False, "Can't convert %s to a ctype" % native_type

class NativeFunctionPointer:
    def __init__(self, fname, fp, input_types, output_type):
        self.fp = fp
        self.fname = fname
        self.input_types = input_types
        self.output_type = output_type
        self._ctypes_cache = None

    def __call__(self, *args):
        if self._ctypes_cache is None:
            argtypes = (native_to_ctype(x) for x in 
                                [self.output_type] + self.input_types)

            self._ctypes_cache = ctypes.CFUNCTYPE(*argtypes)(self.fp)
        
        try:
            return self._ctypes_cache(*args)
        except:
            print "can't call ", self, " with ", args
            raise

    def __repr__(self):
        return "NativeFunctionPointer(name=%s,addr=%x,in=%s,out=%s)" \
            % (self.fname, self.fp, [str(x) for x in self.input_types], str(self.output_type))

_all_compilers_ever = []
class Compiler:
    def __init__(self):
        #we need to keep this object alive - some bug in llvmlite causes
        #us to segfault if we release it
        _all_compilers_ever.append(self)

        self.engine, self.module_pass_manager = create_execution_engine()
        self.converter = native_ast_to_llvm.Converter()

    def add_functions(self, functions):
        if not functions:
            return {}

        module = self.converter.add_functions(functions)

        mod = llvm.parse_assembly(module)
        mod.verify()

        # Now add the module and make sure it is ready for execution
        self.engine.add_module(mod)

        self.module_pass_manager.run(mod)

        self.engine.finalize_object()

        # Look up the function pointer (a Python int)
        result = {}

        for fname in functions:
            func_ptr = self.engine.get_function_address(fname)
            input_types = [x[1] for x in functions[fname].args]
            output_type = functions[fname].output_type

            result[fname] = NativeFunctionPointer(fname, func_ptr, 
                                                  input_types, output_type)

        return result

