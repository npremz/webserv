let loginForm = document.querySelector("#login-form");
let responseSection = document.getElementById('response');
let responseBloc = document.getElementById('responseBloc');
let responseCross = responseSection.querySelector('.section__cross');
let loginSection = document.querySelector(".section--login");

responseCross.addEventListener("click", () => {
    responseSection.classList.add('section--hidden')
})

loginForm.addEventListener("submit", (e) => {
    e.preventDefault();

    var form = e.target;
    var params = [
      "login=" + encodeURIComponent(form.login.value),
      "password=" + encodeURIComponent(form.password.value)
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
