<?php
require_once 'firebaseLib.php';
// --- Aqui se asigna la URL de la base de datos en Firebase
$url = 'https://ecosens-7c798-default-rtdb.firebaseio.com/';
// --- Consume el Token de la base de datos en la siguiente linea
$token = 'd9l8a17U5jGgPANChOp6LIhhrwYezKyt2Oq8VNvX';
// --- Desde aquí obtiene el parametro que se enviará por http GET
$arduino_data = $_GET['arduino_data'];
// --- $arduino_data_post = $_POST['name'];
// --- Aquí asigna la parte de la estrucutra de la base de datos donde irá el parámetro
$firebasePath = '/MedicionesAudio/1/db';
/// --- Generar llamadas
$fb = new fireBase($url, $token);
$response = $fb->set($firebasePath, $arduino_data);
sleep(2);
?>