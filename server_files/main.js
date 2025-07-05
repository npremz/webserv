/*
 * main.js - Logique de test pour Webserv
 *
 * Ce fichier gère :
 *  - L'upload de fichiers (POST /)
 *  - Le listing et la suppression de fichiers (GET/DELETE /images)
 *  - L'appel de scripts CGI (GET/POST)
 *  - Les tests avancés (erreur 404, redirection, autoindex, POST CGI, limite upload)
 *
 * Chaque action utilisateur déclenche une requête HTTP correspondante (voir exemples dans les commentaires).
 */

// --- Upload de fichier (POST /) ---
// Exemple de requête générée :
// POST / HTTP/1.1
// Content-Type: multipart/form-data; boundary=----WebKitFormBoundary...
// ... (données du fichier)
const uploadForm = document.getElementById('uploadForm');
if (uploadForm) uploadForm.onsubmit = async (e) => {
    e.preventDefault();
    const form = e.target;
    const data = new FormData(form);
    const msg = document.getElementById('uploadMsg');
    msg.textContent = 'Envoi en cours...';
    try {
        const res = await fetch('/', { method: 'POST', body: data });
        if (res.ok) {
            msg.textContent = 'Fichier envoyé avec succès !';
            msg.className = 'text-green-600';
        } else {
            msg.textContent = 'Erreur: ' + res.status + ' ' + (await res.text());
            msg.className = 'text-red-600';
        }
    } catch (err) {
        msg.textContent = 'Erreur réseau';
        msg.className = 'text-red-600';
    }
};

// --- Listing et suppression de fichiers (GET/DELETE /images) ---
// Exemple de requête GET générée :
// GET /images/ HTTP/1.1
// Exemple de requête DELETE générée :
// DELETE /images/nom_du_fichier HTTP/1.1
async function refreshFiles() {
    const list = document.getElementById('fileList');
    const msg = document.getElementById('fileMsg');
    list.innerHTML = '<li>Chargement...</li>';
    try {
        const res = await fetch('/images/');
        if (!res.ok) throw new Error('HTTP ' + res.status);
        const html = await res.text();
        // Parse autoindex HTML (simple)
        const files = Array.from(html.matchAll(/<a href="([^\"]+)">([^<]+)<\/a>/g))
            .filter(m => m[2] !== '../')
            .map(m => m[2]);
        list.innerHTML = '';
        if (files.length === 0) list.innerHTML = '<li class="text-gray-500">Aucun fichier</li>';
        files.forEach(file => {
            const li = document.createElement('li');
            li.className = 'flex items-center gap-2 fade-in';
            li.innerHTML = `<span class="flex-1">${file}</span>
        <button class="btn btn-danger" data-file="${file}">Supprimer</button>
        <a href="/images/${encodeURIComponent(file)}" target="_blank" class="btn btn-info">Voir</a>`;
            // Suppression fichier
            // DELETE /images/nom_du_fichier HTTP/1.1
            li.querySelector('button').onclick = async () => {
                if (!confirm('Supprimer ' + file + ' ?')) return;
                const delRes = await fetch('/images/' + encodeURIComponent(file), { method: 'DELETE' });
                if (delRes.ok) {
                    li.classList.add('opacity-50');
                    setTimeout(refreshFiles, 500);
                } else {
                    alert('Erreur suppression: ' + delRes.status);
                }
            };
            list.appendChild(li);
        });
    } catch (err) {
        list.innerHTML = '';
        msg.textContent = 'Erreur chargement: ' + err.message;
        msg.className = 'text-red-600';
    }
}
const refreshBtn = document.getElementById('refreshBtn');
if (refreshBtn) refreshBtn.onclick = refreshFiles;
refreshFiles();

// --- Appel CGI (GET ou POST sur un script) ---
// Exemple de requête générée :
// GET /scripts/test.py?foo=bar&baz=qux HTTP/1.1
// ou
// POST /scripts/test.py HTTP/1.1
// Content-Type: application/x-www-form-urlencoded
// foo=bar&baz=qux
const cgiForm = document.getElementById('cgiForm');
if (cgiForm) cgiForm.onsubmit = async (e) => {
    e.preventDefault();
    const form = e.target;
    const script = form.script.value.trim();
    const params = form.params.value.trim();
    const msg = document.getElementById('cgiMsg');
    const result = document.getElementById('cgiResult');
    msg.textContent = 'Appel CGI...';
    result.textContent = '';
    let url = script;
    if (params) url += '?' + params;
    try {
        const res = await fetch(url);
        const text = await res.text();
        if (res.ok) {
            msg.textContent = 'Réponse CGI :';
            msg.className = 'text-green-600';
        } else {
            msg.textContent = 'Erreur CGI: ' + res.status;
            msg.className = 'text-red-600';
        }
        result.textContent = text;
    } catch (err) {
        msg.textContent = 'Erreur réseau';
        msg.className = 'text-red-600';
    }
};

// --- Tests avancés (erreurs, redirections, limites, etc.) ---
// Chaque bouton déclenche une requête HTTP spécifique pour tester le serveur.

// Test 404 : GET /notfound123 HTTP/1.1
const test404 = document.getElementById('test404');
if (test404) test404.onclick = async () => {
    const msg = document.getElementById('msg404');
    msg.textContent = '...';
    try {
        const res = await fetch('/notfound123');
        msg.textContent = 'Status: ' + res.status;
        msg.className = res.status === 404 ? 'text-green-600' : 'text-red-600';
    } catch (e) { msg.textContent = 'Erreur réseau'; msg.className = 'text-red-600'; }
};

// Test redirection : GET /old-path HTTP/1.1
const testRedirect = document.getElementById('testRedirect');
if (testRedirect) testRedirect.onclick = async () => {
    const msg = document.getElementById('msgRedirect');
    msg.textContent = '...';
    try {
        const res = await fetch('/old-path', { redirect: 'manual' });
        msg.textContent = 'Status: ' + res.status + (res.status === 301 ? ' (redirection OK)' : '');
        msg.className = res.status === 301 ? 'text-green-600' : 'text-red-600';
    } catch (e) { msg.textContent = 'Erreur réseau'; msg.className = 'text-red-600'; }
};

// Test autoindex : GET /images/ HTTP/1.1
const testAutoindex = document.getElementById('testAutoindex');
if (testAutoindex) testAutoindex.onclick = async () => {
    const msg = document.getElementById('msgAutoindex');
    msg.textContent = '...';
    try {
        const res = await fetch('/images/');
        const html = await res.text();
        msg.textContent = html.includes('<a href=') ? 'Autoindex détecté' : 'Pas d\'autoindex';
        msg.className = html.includes('<a href=') ? 'text-green-600' : 'text-red-600';
    } catch (e) { msg.textContent = 'Erreur réseau'; msg.className = 'text-red-600'; }
};

// Test POST CGI : POST /scripts/test.py HTTP/1.1
const testPostCgi = document.getElementById('testPostCgi');
if (testPostCgi) testPostCgi.onclick = async () => {
    const msg = document.getElementById('msgPostCgi');
    msg.textContent = '...';
    try {
        const res = await fetch('/uploads/', { method: 'POST', body: new URLSearchParams({ foo: 'bar' }) });
        const text = await res.text();
        msg.textContent = 'Status: ' + res.status + (res.ok ? ' (OK)' : '');
        msg.className = res.ok ? 'text-green-600' : 'text-red-600';
    } catch (e) { msg.textContent = 'Erreur réseau'; msg.className = 'text-red-600'; }
};

// Test limite upload : POST / (fichier > 4Mo)
const testBigUpload = document.getElementById('testBigUpload');
if (testBigUpload) testBigUpload.onclick = async () => {
    const msg = document.getElementById('msgBigUpload');
    msg.textContent = '...';
    try {
        // Génère un fichier > 4Mo
        const bigBlob = new Blob([new Uint8Array(4.5 * 1024 * 1024)], { type: 'application/octet-stream' });
        const data = new FormData();
        data.append('file', bigBlob, 'bigfile.bin');
        const res = await fetch('/', { method: 'POST', body: data });
        msg.textContent = 'Status: ' + res.status + (res.status === 413 ? ' (Limite atteinte)' : '');
        msg.className = res.status === 413 ? 'text-green-600' : 'text-red-600';
    } catch (e) { msg.textContent = 'Erreur réseau'; msg.className = 'text-red-600'; }
};

// --- Affichage du nom du fichier sélectionné pour l'input file custom ---
const fileInput = document.getElementById('file-upload');
const fileLabel = document.querySelector('label.file-label');
const selectedFile = document.getElementById('selectedFile');
if (fileInput && fileLabel && selectedFile) {
    fileLabel.onclick = () => fileInput.click();
    fileInput.onchange = () => {
        selectedFile.textContent = fileInput.files.length ? fileInput.files[0].name : '';
    };
} 
