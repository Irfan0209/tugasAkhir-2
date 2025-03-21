<?php
include "koneksi.php";

$limit = 10; // Jumlah data per halaman
$page = isset($_GET['page']) ? $_GET['page'] : 1;
$start = ($page - 1) * $limit;

// Ambil total data
$resultTotal = $conn->query("SELECT COUNT(*) AS total FROM logs");
$totalData = $resultTotal->fetch_assoc()['total'];
$totalPages = ceil($totalData / $limit);

// Ambil data untuk halaman ini
$sql = "SELECT * FROM logs ORDER BY id DESC LIMIT $start, $limit";
$result = $conn->query($sql);
?>

<!DOCTYPE html>
<html>

<style>
    /* Tambahan untuk tombol kanan atas */
    .top-right {
        width: 90%;
        margin: 20px auto;
        text-align: right;
    }
</style>

<head>
    <title>Monitoring Data Paket</title>
    <style>
        body { font-family: Arial, sans-serif; background-color: #f4f4f4; }
        h2 { text-align: center; }
        table { margin: auto; border-collapse: collapse; width: 90%; background-color: #fff; }
        th, td { border: 1px solid #ddd; padding: 10px; text-align: center; }
        th { background-color: #3498db; color: white; }
        tr:nth-child(even) { background-color: #f2f2f2; }
        tr:hover { background-color: #ddd; }
        .pagination { text-align: center; margin-top: 20px; }
        .pagination a { margin: 0 5px; padding: 8px 12px; background-color: #3498db; color: white; text-decoration: none; }
        .pagination a.active { background-color: #2c3e50; }
        .pagination a:hover { background-color: #2980b9; }
        .btn { background-color: #2ecc71; color: white; padding: 10px 15px; text-decoration: none; border-radius: 5px; }
    </style>
    <meta http-equiv="refresh" content="10"> <!-- Auto Refresh setiap 10 detik -->
</head>
<body>

<h2>Monitoring Data Paket</h2>
<div class="top-right">
    <a href="export.php" class="btn">Export Data</a>
</div>



<table>
    <tr>
        <th>No</th>
        <th>Tanggal</th>
        <th>Hari</th>
        <th>Waktu</th>
        <th>Label Paket</th>
        <th>Panjang (cm)</th>
        <th>Lebar (cm)</th>
        <th>Tinggi (cm)</th>
        <th>Berat (kg)</th>
    </tr>

<?php
if ($result->num_rows > 0) {
    $no = $start + 1;
    while($row = $result->fetch_assoc()) {
        echo "<tr>";
        echo "<td>".$no++."</td>";
        echo "<td>".$row['tanggal']."</td>";
        echo "<td>".$row['hari']."</td>";
        echo "<td>".$row['waktu']."</td>";
        echo "<td>".$row['label']."</td>";
        echo "<td>".$row['panjang']."</td>";
        echo "<td>".$row['lebar']."</td>";
        echo "<td>".$row['tinggi']."</td>";
        echo "<td>".$row['berat']."</td>";
        echo "</tr>";
    }
} else {
    echo "<tr><td colspan='9'>Belum ada data</td></tr>";
}
$conn->close();
?>
</table>

<!-- Pagination -->
<div class="pagination">
<?php
for($i = 1; $i <= $totalPages; $i++) {
    echo '<a '.($i == $page ? 'class="active"' : '').' href="?page='.$i.'">'.$i.'</a>';
}
?>
</div>

</body>
</html>
