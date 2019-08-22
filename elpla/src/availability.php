<?php
setlocale(LC_ALL, 'de_DE.UTF-8');

$PW="";
if (array_key_exists("PW", $_GET)) {
 $PW = $_GET["PW"];
}

$config = array(
  "master.pwd" => "elpla",
  "database.mysql.host" => "localhost",
  "database.mysql.user" => "elpla",
  "database.mysql.password" => "elpla",
  "database.mysql.database" => "elpla",
);

function load_rc($filename, &$config) {
  if (!file_exists($filename)) {
    return ;
  }
  if (($inifile = parse_ini_file($filename, TRUE))) {
    foreach($inifile as $sectionkey => $sectionvalue) {
      foreach($sectionvalue as $key => $value) {
        $config[$sectionkey . "." . $key] = $value;
      }
    }
  }
}

load_rc("/etc/elpla.rc", $config);
if (array_key_exists("HOME", $_ENV)) {
  load_rc($_ENV["HOME"]."/.elpla/elpla.rc", $config);
}

foreach($_ENV as $key => $value) {
  $norkey = strtolower(str_replace("_", ".", $key));
  if (substr($norkey, 0, 3) === "ed.") {
    $config[substr($norkey, 3)] = $value;
  }
}

$MasterPW=$config["master.pwd"];

$mysqli = new mysqli(
   $config["database.mysql.host"],
   $config["database.mysql.user"],
   $config["database.mysql.password"],
   $config["database.mysql.database"]
);

$Admin=false;
if ($PW == $MasterPW)
{
  $Admin=true;
  //PW = ""
}


function Nullen($Zahl, $AnzahlNullen)
{
  $Temp=($Zahl);
  for ($func_i=1; $func_i<=$AnzahlNullen-strlen($Temp); $func_i++)
  {
    $Temp="0" + $Temp;
  }
  return $Temp;
}

$ClosedUntil = null;
$mysqli->real_query("SELECT VALUE FROM state WHERE ID = 'ClosedUntil'");
if ($result = $mysqli->use_result())
{
  if ($row = $result->fetch_row())
  {
    $ClosedUntil = new DateTime($row[0]);
  }
  $result->close();
}
if ($ClosedUntil === null)
{
  throw new Exception("cannot find ClosedUntil");
}
$OpenedUntil = null;
$mysqli->real_query("SELECT VALUE FROM state WHERE ID = 'OpenedUntil'");
if ($result = $mysqli->use_result())
{
  if ($row = $result->fetch_row())
  {
    $OpenedUntil = new DateTime($row[0]);
  }
  $result->close();
}
if ($OpenedUntil === null)
{
  throw new Exception("cannot find OpenedUntil");
}

//Startdatum auf den naechsten Monatsersten setzen
$Startdatum=new DateTime("first day of next month");
if (array_key_exists('SD',$_GET))
{
  if ($_GET["SD"] != "")
  {
    $Startdatum= new DateTime($_GET["SD"]);
  }
}
$Closed=false;
if ($Startdatum < $ClosedUntil)
{
  $Closed=true;
}

$Prevmonth = clone $Startdatum;
$Prevmonth->sub(new DateInterval("P1M"));
$Nextmonth = clone $Startdatum;
$Nextmonth->modify("+ 1 months");
$Lastdayprevmonth = clone $Startdatum;
$Lastdayprevmonth->sub(new DateInterval("P1D"));
$Lastdaythismonth = clone $Nextmonth;
$Lastdaythismonth->sub(new DateInterval("P1D"));



if ($Admin)
{
  if (array_key_exists('ED_neu',$_GET))
  {
    if($_GET["ED_neu"] == "True")
    {
      $ED_Tag = $_GET["ED_Tag"];
      $ED_TZ = $_GET["TZ"];
      $ED_Eltern_ID = $_GET["Eltern_ID"];
      $mysqli->real_query("Delete From elterndienste Where Datum='". $ED_Tag . "' And TZ='" . $ED_TZ . "'");
      $mysqli->real_query("Insert Into elterndienste (Datum, TZ, Eltern_ID) Values ('" . $ED_Tag . "', '" . $ED_TZ . "', " . $ED_Eltern_ID . ")");
    }
  }

  if (array_key_exists('Verpflichtend',$_GET))
  {
    $ED_Verpflichtend = $_GET["Verpflichtend"];
    $ED_TZ = $_GET["TZ"];
    $mysqli->real_query("Select * From ed_verpflichtend Where Datum='" . $ED_Verpflichtend . "' And TZ='" . $ED_TZ . "'");
    $result = $mysqli->store_result();
    if ($result != FALSE)
    {
      if($result->num_rows > 0)
        $mysqli->real_query("Delete From ed_verpflichtend Where Datum='" . $ED_Verpflichtend . "' And TZ='" . $ED_TZ . "'");
      else
        $mysqli->real_query("Insert Into ed_verpflichtend  (Datum, TZ) Values ('" . $ED_Verpflichtend . "', '" . $ED_TZ . "')");
      $result->close();
    }
  }

  if (array_key_exists('Schliesstag_Datum',$_GET))
  {
    if ($_GET["Schliesstag_ToDo"] == "0")
      $mysqli->real_query("Delete From schliesstage Where Tag='" . $_GET[Schliesstag_Datum] . "'");
    else
      $mysqli->real_query("Insert Into schliesstage (Tag, Anlass) Values ('" . $_GET[Schliesstag_Datum] . "', '')");
  }
  
}

//Eltern abfragen
$Eltern = array();
$Eltern_ID = array();
$Eltern_nach_ID = array();

$Eltern[] = "";
$Eltern_ID[] = "";
$mysqli->real_query("SELECT * FROM eltern WHERE Beginn_Mafalda < '". $Nextmonth->format("Y-m-d")."' And Ende_Mafalda > '". $Lastdayprevmonth->format("Y-m-d")."' Order by Name_Mutter");
$result = $mysqli->use_result();

while ($row = $result->fetch_assoc()) 
{
  $Eltern_nach_ID[$row["ID"]] = $row["Name_Mutter"] . " und " . $row["Name_Vater"];
  if ($row["Passwort"] != $PW)
  {
    $Eltern[] = $row["Name_Mutter"] . " und " . $row["Name_Vater"];
    $Eltern_ID[] = $row["ID"];
  }
  else
  {
    $Eltern[0] = $row["Name_Mutter"] . " und " . $row["Name_Vater"];
    $Eltern_ID[0] = $row["ID"];
  }
}

if (array_key_exists('No',$_GET))
{
  $mysqli->real_query("INSERT IGNORE INTO abwesend (Eltern_ID, Datum, Tageszeit) VALUES ('" . $Eltern_ID[0] . "', '" . $_GET["No"] . "', '" . $_GET["TZ"] . "')"); //TODO: Avoid duplicates
}

if (array_key_exists('Da',$_GET))
{
  $mysqli->real_query("DELETE FROM abwesend WHERE Eltern_ID=" . $Eltern_ID[0] . " AND Datum='" . $_GET["Da"] . "' And Tageszeit='" . $_GET["TZ"] . "'");
}

//Schliesstage im relevanten Monat abfragen
$mysqli->real_query("SELECT * FROM schliesstage WHERE Tag > ('". $Lastdayprevmonth->format("Y-m-d")."') And (Tag < '". $Nextmonth->format("Y-m-d")."')");
$result = $mysqli->use_result();

$Schliesstage = "";
while ($row = $result->fetch_assoc())
{
  $Schliesstage .= $row["Tag"].";";
}

//Befreiungen im relevanten Monat abfragen
$mysqli->real_query("SELECT * FROM befreiung WHERE Bis > ('". $Lastdayprevmonth->format("Y-m-d")."') And (Von < '". $Nextmonth->format("Y-m-d")."')");
$result = $mysqli->use_result();
$Befreiungen = ";";
while ($row = $result->fetch_assoc())
{
  $Von = new DateTime($row["Von"]);
  $Bis = new DateTime($row["Bis"]);
  $Befr_Eltern_ID = $row["Eltern_ID"];
  $Grund = $row["Grund"];
  $Befr_Start = clone $Startdatum;
  if($Von > $Befr_Start) 
    $Befr_Start = $Von;
  $Befr_Ende  = clone $Nextmonth;
  if($Bis < $Befr_Ende) 
    $Befr_Ende = $Bis;
  $Befr_Ende = $Befr_Ende->modify( '+1 day' ); 
  $interval = DateInterval::createFromDateString('1 day');
  $period = new DatePeriod($Befr_Start, $interval, $Befr_Ende);
  foreach ( $period as $Befr_Datum )
  {
    $Befreiungen .= (string)($Befr_Eltern_ID) . "-" . $Befr_Datum->format("d") . "-" . $Grund . ";";
  }
}

//Elterndienste im relevanten Monat abfragen
$mysqli->real_query("SELECT * FROM elterndienste WHERE Datum > ('". $Lastdayprevmonth->format("Y-m-d")."') And (Datum < '". $Nextmonth->format("Y-m-d")."')");
$result = $mysqli->use_result();
$Elterndienste = ";";
while ($row = $result->fetch_assoc())
{
  $Datum = new DateTime($row["Datum"]);
  $Elterndienste .= $Datum->format("d") . "-" .  $row["TZ"] . "-" .  $row["Eltern_ID"] . ";";
}

//Verpflichtende Elterndienste im relevanten Monat abfragen
$mysqli->real_query("SELECT * FROM ed_verpflichtend WHERE Datum > ('". $Lastdayprevmonth->format("Y-m-d")."') And (Datum < '". $Nextmonth->format("Y-m-d")."')");
$result = $mysqli->use_result();
$Verpflichtend = ";";
while ($row = $result->fetch_assoc())
{
  $Datum = new DateTime($row["Datum"]);
  $Verpflichtend .= $Datum->format("d") . "-" .  $row["TZ"] . ";";
} 


//Abwesenheitstage im relevanten Monat abfragen
$mysqli->real_query("SELECT * FROM abwesend WHERE Datum > ('". $Lastdayprevmonth->format("Y-m-d")."') And (Datum < '". $Nextmonth->format("Y-m-d")."')");
$result = $mysqli->use_result();
$Abwesend = "";
while ($row = $result->fetch_assoc())
{
  $Datum = new DateTime($row["Datum"]);
  $Abwesend .= $row["Eltern_ID"] . "-" . $Datum->format("d") . "-" .  $row["Tageszeit"] . ";";
}

?>

<html>
<head>

<title>Mafalda</title>

        <style>
            th
            {
                color: white;
                text-align: center;
                vertical-align: bottom;
                height: 180px;
                padding-bottom: 3px;
                padding-left: 5px;
                padding-right: 5px;
            }

            .verticalText
            {
                text-align: center;
                vertical-align: middle;
                width: 20px;
                margin: 0px;
                padding: 0px;
                padding-left: 3px;
                padding-right: 3px;
                padding-top: 10px;
                white-space: nowrap;
                -webkit-transform: rotate(-90deg); 
                -moz-transform: rotate(-90deg);
                -o-transform: rotate(-90deg);
                -ms-transform: rotate(-90deg);
                filter: progid:DXImageTransform.Microsoft.BasicImage(rotation=3);
                transform: rotate(-90deg);                 
            };
        </style>
        
</head>
<body>
<table width="100%"><tr>
<td align="left"><font face="Arial" size="-1">
  
<?php 
setlocale(LC_ALL, "de_DE.UTF-8");
if ($Prevmonth > new DateTime("31.05.2015")) 
  echo '  <a href="?PW=',$PW,'&SD=',$Prevmonth->format("d.m.Y"),'"><< ', strftime("%B %Y", $Prevmonth->getTimestamp()),'</a>'. "\xA"."  \xA";
?>
</font></td>
<td align="right"><font face="Arial" size="-1">
  
<?php 
if ($Nextmonth < $OpenedUntil)
  echo '  <a href="?PW=',$PW,'&SD=',$Nextmonth->format("d.m.Y"),'">', strftime("%B %Y", $Nextmonth->getTimestamp()),' >></a>'. "\xA"."  \xA";
?>
</font></td>
</table>

<?php
if (! $Closed)
  echo '  <h2><font face="Arial">Mafalda - Verfügbarkeit für Elterndienste ',strftime("%B %Y", $Startdatum->getTimestamp()),'</font></h2>'. "\xA";
else
  echo '  <h2><font face="Arial">Mafalda - Elterndienstplan ',strftime("%B %Y", $Startdatum->getTimestamp()),'</font></h2>'. "\xA";
?>

<table>
<tr><th bgcolor="AAAAAA"><font face="Arial" size="-1"><b>Datum</b></font></th>
<?php
if ($Admin || $Closed)
{
  echo '<th bgcolor="AAAAAA"><font face="Arial" size="-1"><b>Vormittags</b></font></th>';
  echo '<th bgcolor="AAAAAA"><font face="Arial" size="-1"><b>Nachmittags</b></font></th>';
} 

if ($Admin || !$Closed)
{
  for ($i=0; $i<count($Eltern); $i++)
  {
    if ($i == 0 && $Eltern[$i] == "")
      $i++;
    if ($i == 0)
      echo '<th bgcolor="444444">'. "\xA";
    else
      echo '  <th bgcolor="AAAAAA">'. "\xA";
    echo '  <div class="verticalText"><font face="Arial" size="-1"><b>',$Eltern[$i],'</b></font></div></th>'. "\xA";
  }
}
?>
  
</tr>

<?php
  $Datum = clone $Startdatum;

  while ($Startdatum->format('m') == $Datum->format('m')) 
  {
    $Schliesstag=false;
    if ($Datum->format('N') == 6 || $Datum->format('N') == 7 || _instr(0,$Schliesstage,$Datum->format("Y-m-d") . ";",0) > 0)
      $Schliesstag=true; 
    echo '  <tr><td', $Schliesstag ? ' style="color:red"' : '','><font face="Arial" size="-1">', $Admin ? '<b><a href="?PW='.$MasterPW.'&Schliesstag_Datum='.$Datum->format("Y-m-d").'&Schliesstag_ToDo='.-(int)(!$Schliesstag).'&SD='.$Startdatum->format("d.m.Y").'"><font color="red">X</font></a></b>' : '',$Datum->format("d.m.Y"),'</font></td>'. "\xA";
    if ($Admin || $Closed)
    {
      if (_instr(0,$Verpflichtend,";" . $Datum->format("d") . "-v;",0) > 0)
        echo "  \xA" . '      <td bgcolor="#ffff00">'. "\xA";
      else
	echo "  \xA" . '      <td>'. "\xA" . "    \xA";
      echo '      <font face="Arial" size="-1">'. "\xA      ";
      if ($Admin)
        echo "<b><a href='?PW=" . $MasterPW . "&Verpflichtend=" . $Datum->format("Y-m-d") . "&TZ=v&SD=" . $Startdatum->format("d.m.Y") . "'>V</a></b> ". "\xA"; 

      $mysqli->real_query("Select * From elterndienste Where Datum='" . $Datum->format("Y-m-d") . "' And TZ='v'");
      $result = $mysqli->use_result();
      if ($result != FALSE)
      {
        while($row = $result->fetch_assoc())
          echo $Eltern_nach_ID[$row["Eltern_ID"]];
	$result->close();
      }

      echo '      </font>'. "\xA";
      echo '    </td>'. "\xA";
      if (_instr(0,$Verpflichtend,";" . $Datum->format("d") . "-n;",0) > 0)
        echo '<td bgcolor="#ffff00">'. "\xA";
      else
        echo '      <td>'. "\xA";

      echo '      <font face="Arial" size="-1">'. "\xA";

      if ($Admin)
        echo "      <b><a href='?PW=" . $MasterPW . "&Verpflichtend=" . $Datum->format("Y-m-d") . "&TZ=n&SD=" . $Startdatum->format("d.m.Y") . "'>V</a></b> ". "\xA";

      $mysqli->real_query("Select * From elterndienste Where Datum='" . $Datum->format("Y-m-d") . "' And TZ='n'");
      $result = $mysqli->use_result();
      if ($result != FALSE )
      {
        while($row = $result->fetch_assoc())
          echo $Eltern_nach_ID[$row["Eltern_ID"]];
	$result->close();
      }

      echo '      </font>'. "\xA";
      echo '    </td>'. "\xA";
    }

    echo "  \xA";
    if ($Admin || ! $Closed)
    {
      for ($i=0; $i<count($Eltern); $i++)
      {
        if ($i == 0 && $Eltern[$i] == "")
          $i++; 
        if ($Schliesstag)
        {
          if ($i == 0)
            echo '  <td bgcolor="444444">';
          else
            echo '    <td bgcolor="AAAAAA">';
          echo '</td>'. "\xA";
        }
        else
        {
          if (_instr(0,$Befreiungen,";" . ($Eltern_ID[$i]) . "-" . ($Datum->format("d")) . "-",0) > 0) 
          {
            $Grund=substr($Befreiungen,_instr(0,$Befreiungen,";" . ($Eltern_ID[$i]) . "-" . ($Datum->format("d")) . "-",0) + strlen(";" . ($Eltern_ID[$i]) . "-" . ($Datum->format("d")) . "-")-1,_instr(_instr(0,$Befreiungen,";" . ($Eltern_ID[$i]) . "-" . ($Datum->format("d")) . "-",0) + 1,$Befreiungen,";",0)-_instr(0,$Befreiungen,";" . ($Eltern_ID[$i]) . "-" . ($Datum->format("d")) . "-",0)-strlen(";" . ($Eltern_ID[$i]) . "-" . ($Datum->format("d")) . "-"));
            if ($i == 0)
              echo '      <td bgcolor="444444">';
            else
              echo '      <td bgcolor="AAAAAA">';
            echo '<img src="fueller.gif" title="',$Grund,'" width="32" height="18"></td>'. "\xA";
          }
	  else
	  {
            echo '        <td>'. "\xA";
	    echo '        <table border="0" cellspacing="1" cellpadding="1"><tr>'. "\xA";
	    if (_instr(0,$Abwesend,";" . ($Eltern_ID[$i]) . "-" . ($Datum->format("d")) . "-v;",0) > 0) 
	    {
              if ($i == 0)
	        echo '        <td bgcolor="#990000"><a href="?PW=',$PW,'&Da=',$Datum->format("Y-m-d"),'&TZ=v&SD=',$Startdatum->format("d.m.Y"),'"><img src="fueller.gif" width="30" height="8"></a></td>'. "\xA";
              else
                echo '        <td bgcolor="#FF8888"><img src="fueller.gif" width="30" height="8"></td>'. "\xA";
	    } 
	    else
	    {
              if ($i == 0)
	        echo '        <td bgcolor="#009900"><a href="?PW=',$PW,'&No=',$Datum->format("Y-m-d"),'&TZ=v&SD=',$Startdatum->format("d.m.Y"),'"><img src="fueller.gif" width="30" height="8"></a></td>'. "\xA";
              else
	      {
                if ($Admin && _instr(0,$Elterndienste,";" . $Datum->format("d") . "-v-" . $Eltern_ID[$i] . ";",0) > 0)
	          echo "        \xA" . '           <td bgcolor="#009900">'. "\xA";
	        else if ($Admin && _instr(0,$Elterndienste,";" . $Datum->format("d") . "-v-", 0) > 0)
	          echo "        \xA" . '           <td bgcolor="#AAAAAA">'. "\xA";
	        else
	          echo "        \xA" . '              <td bgcolor="#88FF88">'. "\xA";
	        if ($Admin)
	          echo '            <a href="?PW=',$MasterPW,'&ED_neu=True&ED_Tag=',$Datum->format("Y-m-d"),'&TZ=v&Eltern_ID=',$Eltern_ID[$i],'&SD=',$Startdatum->format("d.m.Y"),'" border="0">';
                echo '<img src="fueller.gif" width="30" height="8">';
	        if ($Admin)
	          echo '</a>'. "\xA";
		else
	          echo "\xA";
	        echo '            </td>'. "\xA";
	      }
            }
            echo "          \xA". '        </tr>'. "\xA";
	    echo '        <tr>'. "\xA";
	    if (_instr(0,$Abwesend,";" . ($Eltern_ID[$i]) . "-" . ($Datum->format("d")) . "-n;",0) > 0)
	    {
	      if ($i == 0)
	        echo '        <td bgcolor="#990000"><a href="?PW=',$PW,'&Da=',$Datum->format("Y-m-d"),'&TZ=n&SD=',$Startdatum->format("d.m.Y"),'"><img src="fueller.gif" width="30" height="8"></a></td>'. "\xA";
              else
                echo '        <td bgcolor="#FF8888"><img src="fueller.gif" width="30" height="8"></td>'. "\xA";
	    }
	    else
	    {
              if ($i == 0)
	        echo '        <td bgcolor="#009900"><a href="?PW=',$PW,'&No=',$Datum->format("Y-m-d"),'&TZ=n&SD=',$Startdatum->format("d.m.Y"),'"><img src="fueller.gif" width="30" height="8"></a></td>'. "\xA";
              else
	      {
                if( $Admin && _instr(0,$Elterndienste,";" . $Datum->format("d") . "-n-" . $Eltern_ID[$i] . ";",0) > 0)
	          echo "        \xA" . '              <td bgcolor="#009900">'. "\xA";
	        else if ($Admin && _instr(0,$Elterndienste,";" . $Datum->format("d") . "-n-", 0) > 0)
	          echo "        \xA" . '              <td bgcolor="#AAAAAA">'. "\xA";
		else
		  echo "        \xA" . '              <td bgcolor="#88FF88">'. "\xA";

		echo "            ";
		if ($Admin)
                  echo '<a href="?PW=',$MasterPW,'&ED_neu=True&ED_Tag=',$Datum->format("Y-m-d"),'&TZ=n&Eltern_ID=',$Eltern_ID[$i],'&SD=',$Startdatum->format("d.m.Y"). '">';
		echo '<img src="fueller.gif" width="30" height="8">';
                if ($Admin)
                  echo '</a>'. "\xA";
		else
	          echo "\xA";
                echo '            </td>'. "\xA";
	      }
            }
	    echo "          \xA".'        </tr></table>'. "\xA";
	    echo '        </td>'. "\xA";
            echo "      \xA";
          }
        }
      }
      echo "      \xA";
    }
    echo '  </tr>'. "\xA";
    $Datum = $Datum->add(new DateInterval("P1D"));
  }


  function _instr($start,$str1,$str2,$mode) 
  {
    if ($mode) { $str1=strtolower($str1); $str2=strtolower($str2); }
    $retval=strpos($str1,$str2,$start);
    return ($retval===false) ? 0 : $retval+1;
  }
?>
</table>
</body>
</html>

<?php
  $mysqli->close();
?>

