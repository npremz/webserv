let uploadForm = document.getElementById("upload-form");
let formFiles = document.querySelector(".form__files");
let fileBox = document.querySelector(".file-box");

let responseSection = document.getElementById('response');
let responseBloc = document.getElementById('responseBloc');
let responseCross = responseSection.querySelector('.section__cross');

formFiles.addEventListener('change', function(event) {
    var files = event.target.files;
    fileBox.innerHTML = ""; // On vide l'affichage précédent

    if (!files.length) {
        fileBox.textContent = "Aucun fichier sélectionné";
    } else {
        var ul = document.createElement('ul');
        for (var i = 0; i < files.length; i++) {
            var li = document.createElement('li');
            li.textContent = files[i].name; // Affiche le nom du fichier
            ul.appendChild(li);
        }
        fileBox.appendChild(ul);
    }
});

uploadForm.addEventListener("submit", (e) => {
    e.preventDefault();

    var form = e.target;
    var data = new FormData(form)

    
    responseBloc.innerText= "";
    responseSection.classList.remove('section--hidden')

    fetch('/cgi-bin/multipart.py', {
        method: 'POST',
        body: data
    })
    .then(response => {
        if (!response.ok) {
            return response.text().then(text => {
                throw new Error(`${response.status}\n${text}`);
            });
        }
        return response.text();
    })
    .then(text => {
        responseBloc.innerText = "Upload status: " + text;
    })
    .catch(error => {
        responseBloc.innerText = error;
    });
})

responseCross.addEventListener("click", (e) => {
    responseSection.classList.add("section--hidden");
})
