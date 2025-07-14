
let refreshAutoIndexBtn = document.querySelector("#refreshAutoindex");
let refreshAutoIndexSpinner = document.querySelector("#refreshAutoindexSpinner");
let autoindexContainer = document.querySelector("#autoindexContainer");

let sectionsTestGet = document.querySelectorAll(".section-test-get");

let getInput = document.querySelector("#getInput");
let sendGetBtn = document.querySelector("#sendGetBtn")
let getSpinner = document.querySelector("#getSpinner");
let getContainer = document.querySelector("#getContainer");

getAutoindex("/images", refreshAutoIndexSpinner, autoindexContainer);

sectionsTestGet.forEach(element => {
    let refresh = element.querySelector("button");
    let spinner = element.querySelector("svg");
    let container = element.querySelector(".code-container");
    let path = element.getAttribute("data-path");

    simpleGet(path, spinner, container);

    refresh.addEventListener("click", () => {
        simpleGet(path, spinner, container);
    })
});

refreshAutoIndexBtn.addEventListener("click", () => {
    getAutoindex("/images", refreshAutoIndexSpinner, autoindexContainer);
})

sendGetBtn.addEventListener("click", () => {
    simpleGet(getInput.value, getSpinner, getContainer);
})

function getAutoindex(path, spinner, dist) {
    dist.innerHTML = "";
    spinner.classList.remove("hidden");
    fetch(window.location.origin + path)
        .then(response => response.text())
        .then(data => {
            console.log(data);
            var parser = new DOMParser();
            var doc = parser.parseFromString(data, 'text/html');
            var links = doc.querySelectorAll('ul li a');

            links.forEach((link) => {
                var nouveauLien = link.cloneNode(true);
                dist.appendChild(nouveauLien);
            });

            spinner.classList.add("hidden");
        })
        .catch(error => {
            console.error('Erreur :', error);
        });
}

function simpleGet(path, spinner, dist) {
    spinner.classList.remove("hidden");
    fetch(window.location.origin + path)
        .then(response => response.text())
        .then(data => {
            console.log(data);
            dist.innerHTML = '<pre>' + escapeHTML(data) + '</pre>';
            spinner.classList.add("hidden");
        })
        .catch(error => {
            console.error('Erreur :', error);
        });
}

function escapeHTML(html) {
  var span = document.createElement('span');
  span.appendChild(document.createTextNode(html));
  return span.innerHTML;
}
