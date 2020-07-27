"use strict";

export const app = new Vue({
    el: "#root",
    data: {
        page: 1,
        currentClass:{}
    },
    methods: {
        ChangePage: function(value){
            //Remove the id from the current page
            let currentLink = document.querySelector("#CurrentPage");
            currentLink.removeAttribute("id");
            
            //Add the id to the new current page
            let links = document.querySelector("#Navigation").querySelectorAll("li");
            links[value].setAttribute("id", "CurrentPage");
            
            this.page = value;
        },
        DisplayClass: function(value){
            let file = new XMLHttpRequest();
            file.open("GET", value + ".json", false);
            
            file.onreadystatechange = () => {
                if(file.readyState == 4){
                    if(file.status == 200 || file.status == 0){
                        let text = file.responseText;
                        this.currentClass = JSON.parse(text);
                    }
                }
            }
            file.send(null);
        }
    },
});

export function Init(){
}