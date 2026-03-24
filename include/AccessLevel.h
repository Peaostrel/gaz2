#ifndef ACCESSLEVEL_H // Защита от двойного включения
#define ACCESSLEVEL_H // Установка макроса

enum class AccessLevel { // Перечисление уровней доступа
    GUEST, // Уровень гостя (0)
    USER,  // Уровень пользователя (1)
    ADMIN  // Уровень администратора (2)
}; // Конец перечисления

#endif // ACCESSLEVEL_H // Конец защиты