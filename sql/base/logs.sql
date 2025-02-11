# Logs database

DROP TABLE IF EXISTS `logs_quests`;
CREATE TABLE `logs_quests` (
  `account` int(11) NOT NULL COMMENT 'account guid',
  `character` int(11) NOT NULL COMMENT 'character guid',
  `plevel` tinyint(3) NOT NULL COMMENT 'player level',
  `quest_id` mediumint(8) NOT NULL COMMENT 'quest id',
  `e_timer` mediumint(8) NOT NULL COMMENT 'elapsed timer'
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `logs_db_version`;
CREATE TABLE `logs_db_version` (
  `required_s2433_01_logs_anticheat` bit(1) DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Last applied sql update to DB';

LOCK TABLES `logs_db_version` WRITE;
/*!40000 ALTER TABLE `logs_db_version` DISABLE KEYS */;
INSERT INTO `logs_db_version` VALUES
(NULL);
/*!40000 ALTER TABLE `logs_db_version` ENABLE KEYS */;
UNLOCK TABLES;

DROP TABLE IF EXISTS `logs_anticheat`;

CREATE TABLE `logs_anticheat` (
  `id` INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `time` DATETIME DEFAULT NULL,
  `realm` INT(10) UNSIGNED NOT NULL,
  `account` INT(10) UNSIGNED NOT NULL,
  `ip` VARCHAR(16) NOT NULL,
  `fingerprint` int(10) unsigned NOT NULL,
  `actionMask` INT(10) UNSIGNED DEFAULT NULL,
  `player` VARCHAR(32) NOT NULL,
  `info` VARCHAR(512) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `account` (`account`),
  KEY `ip` (`ip`),
  KEY `time` (`time`),
  KEY `realm` (`realm`)
) ENGINE=INNODB AUTO_INCREMENT=34 DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `logs_spamdetect`;

CREATE TABLE `logs_spamdetect` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `realm` int(10) unsigned NOT NULL,
  `accountId` int(11) DEFAULT '0',
  `fromGuid` bigint unsigned DEFAULT '0',
  `fromIP` varchar(16) NOT NULL,
  `fromFingerprint` int(10) unsigned NOT NULL,
  `comment` varchar(8192) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`),
  KEY `guid` (`fromGuid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
