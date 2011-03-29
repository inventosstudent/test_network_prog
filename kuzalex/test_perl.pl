#!/usr/bin/perl

print "Ваше имя?\n";    # Приглашение ввести имя.
$name = <STDIN>;        # Ввод имени с клавиатуры.


$~ = NAME_FORMAT_TOP;     
write;                  
$~ = NAME_FORMAT;       # Назначение формата вывода.
write;                  # Вывод приветствия.



$~ = NAME_FORMAT_BOTTOM;    # Вывод нижней разделительной черты.
write;

format NAME_FORMAT=
Привет, @>>>>>>>>>>>!         
$name                    
.

format NAME_FORMAT_TOP=
================================
Сообщение Peri-программы
.


format NAME_FORMAT_BOTTOM=
================================
.
