/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;

CREATE TABLE `abwesend` (
  `Eltern_ID` int(11) NOT NULL,
  `Datum` date NOT NULL,
  `Tageszeit` char(1) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `befreiung` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `Von` date NOT NULL,
  `Bis` date NOT NULL,
  `Eltern_ID` int(11) NOT NULL,
  `Grund` text NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;

CREATE TABLE `ed_verpflichtend` (
  `Datum` date NOT NULL,
  `Status` int(11) NOT NULL DEFAULT 0,
  `TZ` char(1) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `eltern` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `Name_Kind` text NOT NULL,
  `Name_Mutter` text NOT NULL,
  `Name_Vater` text NOT NULL,
  `Beginn_Mafalda` date NOT NULL,
  `Ende_Mafalda` date NOT NULL,
  `Mail_Mutter` text NOT NULL,
  `Mail_Vater` text NOT NULL,
  `Passwort` text NOT NULL,
  `Visit_Hours` text DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;

CREATE TABLE `elterndienste` (
  `Datum` date NOT NULL,
  `TZ` char(1) NOT NULL,
  `Eltern_ID` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `fulfilled_shifts` (
  `parent_id` int(11) NOT NULL,
  `date` date DEFAULT NULL,
  `total_hours` decimal(10,1) DEFAULT NULL,
  `from_hour` decimal(10,1) DEFAULT NULL,
  `to_hour` decimal(10,1) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `schliesstage` (
  `Tag` date NOT NULL,
  `Anlass` text NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*!40101 SET character_set_client = @saved_cs_client */;
