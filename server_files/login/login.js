let loginForm = document.querySelector("#login-form");
let responseSection = document.getElementById('response');
let loggedInSection = document.getElementById('loggedIn');
let loggedInSectionTxt = document.querySelector('#loggedIn p');
let responseBloc = document.getElementById('responseBloc');
let responseCross = responseSection.querySelector('.section__cross');
let loginSection = document.querySelector(".section--login");
let logOutBtn = document.querySelector("#logOut");

async function updateUIFromSession()
{
    try {
        fetch('/cgi-bin/login.py?format=json', {
            headers: { "Accept": "application/json" }
        })
        .then(response => {
            if (!response.ok) {
                return response.text().then(text => {
                    throw new Error(`${response.status}\n${text}`);
                });
            }
            return response.json();
        })
        .then (json => {
            if (json.loggedIn) {
                loginSection.classList.add('section--hidden');
                loggedInSectionTxt.textContent = "Connected as: " + json.login || "";
                loggedInSection.classList.remove('section--hidden');
            } else {
                loggedInSection.classList.add('section--hidden');
                loggedInSectionTxt.textContent = "";
                loginSection.classList.remove('section--hidden');
            }
        })
    } catch (e) {
        loggedInSection.classList.add('section--hidden');
        loginSection.classList.remove('section--hidden');
        console.error(e);
    }
}

updateUIFromSession();

logOutBtn.addEventListener("click", async () => {
    try {
      const resp = await fetch('/cgi-bin/login.py?action=logout', { method: 'GET' });
      // Même si le serveur répond en texte, on met à jour l’UI juste après
      await updateUIFromSession();
    } catch (e) {
      console.error(e);
    }
});

responseCross.addEventListener("click", () => {
    responseSection.classList.add('section--hidden')
});

loginForm.addEventListener("submit", (e) => {
    e.preventDefault();

    var form = e.target;
    var params = [
      "login=" + encodeURIComponent(form.login.value),
      "password=" + encodeURIComponent(form.password.value),
      "action=login"
    ].join("&");

    form.password.value = "";
    
    responseBloc.innerText= "";
    responseSection.classList.remove('section--hidden')

    fetch('/cgi-bin/login.py', {
        method: 'POST',
        headers: {
            "Content-Type": "application/x-www-form-urlencoded"
        },
        body: params
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
        responseBloc.innerText = "Connection status: " + text;
        loginSection.classList.add('section--hidden');
    })
    .catch(error => {
        responseBloc.innerText = error;
    });
})
