-- MySQL dump 10.13  Distrib 9.2.0, for Win64 (x86_64)
--
-- Host: localhost    Database: gp2025
-- ------------------------------------------------------
-- Server version	9.2.0

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Current Database: `gp2025`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `gp2025` /*!40100 DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci */ /*!80016 DEFAULT ENCRYPTION='N' */;

USE `gp2025`;

--
-- Table structure for table `player_info`
--

DROP TABLE IF EXISTS `player_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `player_info` (
  `id` int unsigned NOT NULL,
  `character_type` tinyint unsigned NOT NULL,
  `pos_x` float NOT NULL DEFAULT '0',
  `pos_y` float NOT NULL DEFAULT '0',
  `pos_z` float NOT NULL DEFAULT '0',
  `yaw` float NOT NULL DEFAULT '0',
  `collision_radius` float NOT NULL DEFAULT '50',
  `attack_radius` float NOT NULL DEFAULT '150',
  `fov_angle` float NOT NULL DEFAULT '90',
  `level` int unsigned NOT NULL DEFAULT '1',
  `exp` float NOT NULL DEFAULT '0',
  `max_exp` float NOT NULL DEFAULT '100',
  `hp` float NOT NULL DEFAULT '100',
  `max_hp` float NOT NULL DEFAULT '100',
  `damage` float NOT NULL DEFAULT '10',
  `crt_rate` float NOT NULL DEFAULT '0.5',
  `crt_value` float NOT NULL DEFAULT '1.5',
  `dodge` float NOT NULL DEFAULT '0.1',
  `speed` float NOT NULL DEFAULT '0',
  `gold` int unsigned NOT NULL DEFAULT '0',
  `skill1_gid` tinyint unsigned NOT NULL DEFAULT '0',
  `skill1_level` int unsigned NOT NULL DEFAULT '0',
  `skill2_gid` tinyint unsigned NOT NULL DEFAULT '0',
  `skill2_level` int unsigned NOT NULL DEFAULT '0',
  `skill3_gid` tinyint unsigned NOT NULL DEFAULT '0',
  `skill3_level` int unsigned NOT NULL DEFAULT '0',
  `current_quest_type` tinyint unsigned NOT NULL DEFAULT '0',
  `current_quest_status` tinyint unsigned NOT NULL DEFAULT '0',
  `equip_weapon` tinyint unsigned NOT NULL DEFAULT '0',
  `equip_helmet` tinyint unsigned NOT NULL DEFAULT '0',
  `equip_chest` tinyint unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  CONSTRAINT `player_info_ibfk_1` FOREIGN KEY (`id`) REFERENCES `users` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `player_info`
--

LOCK TABLES `player_info` WRITE;
/*!40000 ALTER TABLE `player_info` DISABLE KEYS */;
/*!40000 ALTER TABLE `player_info` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `user_friends`
--

DROP TABLE IF EXISTS `user_friends`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `user_friends` (
  `user_id` int unsigned NOT NULL,
  `friend_id` int unsigned NOT NULL,
  `status` tinyint(1) NOT NULL DEFAULT '0',
  `requester_id` int unsigned DEFAULT NULL,
  PRIMARY KEY (`user_id`,`friend_id`),
  KEY `fk_user_friends_friend` (`friend_id`),
  CONSTRAINT `fk_user_friends_friend` FOREIGN KEY (`friend_id`) REFERENCES `users` (`id`) ON DELETE CASCADE,
  CONSTRAINT `fk_user_friends_user` FOREIGN KEY (`user_id`) REFERENCES `users` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `user_friends`
--

LOCK TABLES `user_friends` WRITE;
/*!40000 ALTER TABLE `user_friends` DISABLE KEYS */;
/*!40000 ALTER TABLE `user_friends` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `user_items`
--

DROP TABLE IF EXISTS `user_items`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `user_items` (
  `user_id` int unsigned NOT NULL,
  `item_id` int unsigned NOT NULL,
  `item_type_id` tinyint unsigned NOT NULL,
  PRIMARY KEY (`user_id`,`item_id`),
  CONSTRAINT `user_items_ibfk_1` FOREIGN KEY (`user_id`) REFERENCES `users` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `user_items`
--

LOCK TABLES `user_items` WRITE;
/*!40000 ALTER TABLE `user_items` DISABLE KEYS */;
/*!40000 ALTER TABLE `user_items` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `users`
--

DROP TABLE IF EXISTS `users`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `users` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `login_id` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `password` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `nickname` varchar(24) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `login_id` (`login_id`),
  UNIQUE KEY `nickname` (`nickname`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `users`
--

LOCK TABLES `users` WRITE;
/*!40000 ALTER TABLE `users` DISABLE KEYS */;
/*!40000 ALTER TABLE `users` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2025-07-26  0:58:20
