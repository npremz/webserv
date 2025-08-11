let filesSection = document.querySelector(".section--files");
let errorSection = document.querySelector(".section--error");
let responseSection = document.querySelector(".section--response");

console.log(responseSection)

function getFiles() {
    fetch('/cgi-bin/getFiles.py')
    .then(response => {   
        if (!response.ok) {
            return response.text().then(text => {
                throw new Error(`${response.status}\n${text}`);
            });
        }
        return response.json();
    })
    .then(json => {
        filesSection.innerHTML = "";

        let h2 = document.createElement("h2");
        h2.innerText = `${json.count} items found`;
        filesSection.appendChild(h2);

        json.files.forEach(fileName => {
            let fileDiv = document.createElement("div");

            fileDiv.classList.add("file");
            fileDiv.innerText = fileName + " ";

            let deleteBtn = document.createElement("button");
            deleteBtn.classList.add("btn")
            deleteBtn.innerText = "Delete";
            deleteBtn.onclick = function() {
                fetch(`/cgi-bin/deleteFile.py?file=${encodeURIComponent(fileName)}`, {
                    method: "DELETE"
                })
                .then(response => {
                    if (!response.ok) {
                        return response.text().then(text => {
                            throw new Error(`${response.status}\n${text}`);
                        });
                    }
                    responseSection.classList.remove("section--hidden");
                    responseSection.innerText = "Response status: " + response.status;
                    getFiles();
                })
                .catch(error => {
                    errorSection.classList.remove("section--hidden");
                    errorSection.innerText = error;
                });
            };

            fileDiv.appendChild(deleteBtn);
            filesSection.appendChild(fileDiv);
        });
    })
    .catch(error => {
        errorSection.classList.remove("section--hidden");
        errorSection.innerText = error;
    });
}

getFiles();
