-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Хост: 127.0.0.1
-- Время создания: Май 14 2026 г., 19:20
-- Версия сервера: 10.4.32-MariaDB
-- Версия PHP: 8.2.12

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- База данных: `sa_db`
--

-- --------------------------------------------------------

--
-- Структура таблицы `loot`
--

CREATE TABLE `loot` (
  `id` int(11) NOT NULL,
  `type` int(11) NOT NULL,
  `name` varchar(64) NOT NULL,
  `chance` int(11) NOT NULL,
  `is_model` tinyint(4) NOT NULL DEFAULT 0,
  `value` int(11) NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Дамп данных таблицы `loot`
--

INSERT INTO `loot` (`id`, `type`, `name`, `chance`, `is_model`, `value`) VALUES
(1, 3570, 'Supa-Dupa item reward', 11, 1, 1111),
(2, 3570, 'Normal item reward', 22, 1, 1111),
(3, 3570, 'Rich Bitch money reward', 17, 0, 5555),
(4, 3570, 'Homeless mercy money reward', 50, 0, 2222);

-- --------------------------------------------------------

--
-- Структура таблицы `pending_rewards`
--

CREATE TABLE `pending_rewards` (
  `player_nick` varchar(24) NOT NULL,
  `is_model` tinyint(4) NOT NULL,
  `value` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- --------------------------------------------------------

--
-- Структура таблицы `spawn_containers`
--

CREATE TABLE `spawn_containers` (
  `id` int(10) UNSIGNED NOT NULL,
  `x` float NOT NULL,
  `y` float NOT NULL,
  `z` float NOT NULL,
  `type` int(11) NOT NULL,
  `initial_price` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Дамп данных таблицы `spawn_containers`
--

INSERT INTO `spawn_containers` (`id`, `x`, `y`, `z`, `type`, `initial_price`) VALUES
(1, 3, 3.5, 3, 3570, 100),
(2, 14.14, 14.14, 4, 3570, 100);

--
-- Индексы сохранённых таблиц
--

--
-- Индексы таблицы `loot`
--
ALTER TABLE `loot`
  ADD PRIMARY KEY (`id`);

--
-- Индексы таблицы `spawn_containers`
--
ALTER TABLE `spawn_containers`
  ADD PRIMARY KEY (`id`);

--
-- AUTO_INCREMENT для сохранённых таблиц
--

--
-- AUTO_INCREMENT для таблицы `loot`
--
ALTER TABLE `loot`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;

--
-- AUTO_INCREMENT для таблицы `spawn_containers`
--
ALTER TABLE `spawn_containers`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=3;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
