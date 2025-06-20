<?php
    echo "Content-Type: text/html\r\n\r\n";
    echo "<html><head><title>Test CGI PHP</title></head><body>";
    echo "<h1>Bienvenue sur mon CGI PHP !</h1>";
    echo "<h2>Résumé de l'environnement CGI :</h2>";
    echo "<ul>";
    echo "<li><b>REQUEST_METHOD:</b> " . htmlspecialchars($_SERVER['REQUEST_METHOD']) . "</li>";
    echo "<li><b>SCRIPT_FILENAME:</b> " . htmlspecialchars($_SERVER['SCRIPT_FILENAME']) . "</li>";
    echo "<li><b>QUERY_STRING:</b> " . htmlspecialchars($_SERVER['QUERY_STRING']) . "</li>";
    echo "<li><b>DOCUMENT_ROOT:</b> " . htmlspecialchars($_SERVER['DOCUMENT_ROOT']) . "</li>";
    echo "<li><b>REQUEST_URI:</b> " . htmlspecialchars($_SERVER['REQUEST_URI']) . "</li>";
    echo "<li><b>SERVER_PROTOCOL:</b> " . htmlspecialchars($_SERVER['SERVER_PROTOCOL']) . "</li>";
    echo "</ul>";

    echo "<h2>GET parameters :</h2><ul>";
    foreach ($_GET as $key => $value)
        echo "<li>" . htmlspecialchars($key) . " = " . htmlspecialchars($value) . "</li>";
    echo "</ul>";

    echo "</body></html>";
?>
