/**
 * _NavTab Cell Component
 * NOTE: This should probably just be
 * rolled into the Nav component somehow,
 * or included in its module as a private
 * subcomponent.
 */

//import {Component} from './Component';
//import {h} from 'maquette';

/**
 * About Replacements
 * -------------------
 * This component has one regular
 * replacement:
 * * `child`
 */
class _NavTab extends Component {
    constructor(props, ...args){
        super(props, ...args);

        // Bind context to methods
        this.clickHandler = this.clickHandler.bind(this);
    }

    render(){
        let innerClass = "nav-link";
        if(this.props.extraData.isActive){
            innerClass += " active";
        }
        return (
            h('li', {
                id: this.props.id,
                class: "nav-item",
                "data-cell-id": this.props.id,
                "data-cell-type": "_NavTab"
            }, [
                h('a', {
                    class: innerClass,
                    role: "tab",
                    onclick: this.clickHandler
                }, [this.getReplacementElementFor('child')])
            ])
        );
    }

    clickHandler(event){
        cellSocket.sendString(
            JSON.stringify(this.props.extraData.clickData, null, 4)
        );
    }
}

//export {_NavTab, _NavTab as default};
