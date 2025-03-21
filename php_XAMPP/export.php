<?php
header("Content-Type: text/csv");
header("Content-Disposition: attachment; filename=data.csv");

include "koneksi.php";
$output = fopen("php://output", "w");

// Header CSV pakai titik koma
fputcsv($output, ['Tanggal', 'Hari', 'Waktu', 'Label', 'Panjang', 'Lebar', 'Tinggi', 'Berat'], ';');

// Ambil data dari database
$query = $conn->query("SELECT tanggal, hari, waktu, label, panjang, lebar, tinggi, berat FROM logs");

// Tulis data ke CSV, tanggal dibuat string agar tidak error di Excel
while($row = $query->fetch_assoc()) {
    fputcsv($output, [
        " " . $row['tanggal'], // Petik agar Excel baca sebagai teks
        $row['hari'],
        $row['waktu'],
        $row['label'],
        $row['panjang'],
        $row['lebar'],
        $row['tinggi'],
        $row['berat']
    ], ';');
}

fclose($output);
?>
