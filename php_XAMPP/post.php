<?php
include "koneksi.php"; // Pastikan koneksi ke database dht11

// Set zona waktu ke Jakarta
date_default_timezone_set('Asia/Jakarta');

// Ambil data tanggal, hari, dan waktu
$tanggal = date("Y-m-d");
$hariArray = array("Minggu","Senin","Selasa","Rabu","Kamis","Jumat","Sabtu");
$hari = $hariArray[date("w")];
$waktu = date("H:i:s");

// Contoh data POST dari form atau sensor
$label   = $_POST['label'];   // misal: "PAKET A"
$panjang = $_POST['panjang']; // misal: 30.5
$lebar   = $_POST['lebar'];   // misal: 20.0
$tinggi  = $_POST['tinggi'];  // misal: 15.5
$berat   = $_POST['berat'];   // misal: 3.2 (kg)

// Validasi sederhana (cek semua data terisi)
if(!empty($label) && !empty($panjang) && !empty($lebar) && !empty($tinggi) && !empty($berat)) {

    // Query Insert ke database
    $sql = "INSERT INTO logs (tanggal, hari, waktu, label, panjang, lebar, tinggi, berat)
            VALUES ('$tanggal', '$hari', '$waktu', '$label', '$panjang', '$lebar', '$tinggi', '$berat')";

    if ($conn->query($sql) === TRUE) {
        echo "Data berhasil disimpan";
    } else {
        echo "Error: " . $sql . "<br>" . $conn->error;
    }

} else {
    echo "Data tidak lengkap!";
}

$conn->close();
?>

