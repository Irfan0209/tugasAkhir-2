<?php
// Create Database if not exists
$servername = "localhost";
$username = "root";
$password = "";

// Create connection ke MySQL
$conn = new mysqli($servername, $username, $password);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Buat database baru (nama: dht11)
$sql = "CREATE DATABASE timbangan";
if ($conn->query($sql) === TRUE) {
    echo "Database created successfully<br>";
} else {
    echo "Error creating database: " . $conn->error;
}
$conn->close();

echo "<br>";

// Connect ke database dht11
$conn = new mysqli($servername, $username, $password, "timbangan");
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// SQL untuk create table logs sesuai kebutuhan baru
$sql = "CREATE TABLE logs (
    id INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    tanggal DATE NULL,
    hari VARCHAR(20),
    waktu TIME NULL,
    label VARCHAR(50),
    panjang FLOAT,
    lebar FLOAT,
    tinggi FLOAT,
    berat FLOAT
)";

if ($conn->query($sql) === TRUE) {
    echo "Table logs created successfully";
} else {
    echo "Error creating table: " . $conn->error;
}

$conn->close();
?>
