"use strict";

export const app = new Vue({
    el: "#root",
    data: {
        page: 1,
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
    },
});

export function Init(){
}