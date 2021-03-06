# Урок 1 - Создание контекста OpenGL 3.3 #

![http://i.imgur.com/lDHyLPpl.png](http://i.imgur.com/lDHyLPpl.png)

## Введение ##

Этот урок является вводным к циклу уроков по OpenGL 3.3. Цикл уроков задуман мной как ознакомительный и, в то же время, как справочный материал для всех кто интересуется OpenGL, особенно новой версией 3.3.

Я не буду подробно останавливаться на том, чем OpenGL 3.3 отличается от своих предшественников, если это кому-то интересно то можно ознакомиться с моей прошлой заметкой, в ней кратко рассказано про отличия спецификации OpenGL 3.0 от предыдущих версий, ссылка на заметку - http://usefulbytes.blogspot.com/2009/04/opengl-3.html

В этом и последующих уроках я планирую использовать платформу Windows как базовую, однако, в дальнейшем, возможно портирование исходных кодов на другие платформы, в частности меня интересуют платформы Linux и Mac OS.

Я полагаю, что вы уже знакомы с основами программирование на языке С++, также надеюсь что у вас есть базовые навыки работы с WinAPI, т.к. в текущем уроке используется WinAPI для создания и управления окном, к которому будет привязан контекст OpenGL. Если у вас возникают трудности с освоением материала - не расстраивайтесь, лучше обратитесь к справочной литературе и Интернету за выяснением подробностей по незнакомым конструкциям в коде. Когда мне что-то не понятно я именно так и поступаю, в этом ничего зазорного, знать абсолютно все невозможно :)

Для полноценной работы с функционалом представленным в OpenGL 3.3 нам необходимо, для начала, создать контекст с поддержкой OpenGL 3.3. Далее будут рассмотрены основные этапы, через которые надо пройти, для достижения этой цели.

## Создание контекста OpenGL 3.3 ##

В первую очередь после создания окна нам необходимо определиться с форматом пикселей, который мы будем использовать, поможет нам в этом функция `ChoosePixelFormat` [1](http://msdn.microsoft.com/en-us/library/dd318284.aspx), она определит какой из системных форматов больше всего подходит к интересующему нас формату пикселей:

```
// объявим необходимые переменные
PIXELFORMATDESCRIPTOR pfd;
int format;

// обнулим структуру с описанием формата пикселей и сохраним ее размер
memset(&pfd, 0, sizeof(pfd));
pfd.nSize = sizeof(pfd);

// согласно MSDN версия всегда равна 1
pfd.nVersion = 1;

// нас интересует поддержка окном OpenGL и двойного буфера
pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;

// пиксель с цветовыми компонентами RGBA
pfd.iPixelType = PFD_TYPE_RGBA;

// нам надо 32 бита на цвет, по 8 на каждую компоненту
pfd.cColorBits = 32;

// разрядность буфера глубины определяет его точность, зададим 24 бита
pfd.cDepthBits = 24;

// попробуем выбрать формат один из системных форматов пикселей
format = ChoosePixelFormat(hDC, &pfd);
```

Выбор формата пикселей нужен в первую очередь для того, чтобы установить правильные настройки окна, в котором будет отображаться все, что мы будем выводить на экран в дальнейшем. По хорошему мы должны также проверить, что ключевые для нас настройки формата присутствуют в том формате пикселей, который вернула функция `ChoosePixelFormat`. Однако, судя по моему опыту, ситуация когда система не может предоставить используемый в уроке формат маловероятна. Но все же стоит знать, что ситуация, когда система вернет не запрошенный формат, возможна - например не получится установить точность буфера глубины в 64 бита, система может предоставить 24 бита и сказать об успехе. Учтите это и старайтесь задавать корректные форматы пикселей.

Отдельного внимания заслуживает точность буфера глубины. Для всех уроков которые я собираюсь вам представить хватит точности в 16 бит, т.к. на экране не предвидится больших обозримых пространств или огромных объектов. Но вообще этот параметр выбирается исходя из задачи, если разница между ближней (z-near) и дальней (z-far) плоскостью отсечения небольшая, к примеру вращающийся куб, top-down игра или 2D, то хватит и точности в 16 бит. Ставлю здесь 24 бита, чтобы если кто-то скопирует код не удивлялся потом, почему что-то криво рисуется в его случае :)

После того как мы выбрали подходящий формат пикселей, мы должны установить его для нашего окна, делается это с помощью функции `SetPixelFormat` [2](http://msdn.microsoft.com/en-us/library/dd369049.aspx):

```
// если ни один из системных форматов не подходит или его невозможно применить к окну
if (!format || !SetPixelFormat(hDC, format, &pfd))
{
	LOG_ERROR("Setting pixel format fail (%d)\n", GetLastError());
	return false;
}
```

Как вы уже увидели, для записи сообщений в лог-файл используется макрос `LOG_ERROR`, он подобен функции `printf`. Этот макрос используется в исходных кодах приложенных к этому уроку (ссылки в конце страницы).

В Windows для создания контекста OpenGL 3.3 нам понадобится функция-расширение `wglCreateContextAttribsARB` [3](http://www.opengl.org/registry/specs/ARB/wgl_create_context.txt), доступ к которой мы можем получить посредством специальной функции `wglGetProcAddress` [4](http://msdn.microsoft.com/en-us/library/dd374386.aspx), но для этого нам надо иметь уже готовый контекст OpenGL более старой версии. Поэтому для начала нам надо создать временный контекст OpenGL посредством вызова функции `wglCreateContext` [5](http://msdn.microsoft.com/en-us/library/dd374379.aspx):

```
// создадим временный контекст OpenGL
HGLRC hRCTemp = wglCreateContext(hDC);

// если не получилось создать контекст или контекст не устанавливается для нашего окна
if (!hRCTemp || !wglMakeCurrent(hDC, hRCTemp))
{
	LOG_ERROR("Сreating temp render context fail (%d)\n", GetLastError());
	return false;
}
```

После чего мы уже можем получить доступ к функции `wglCreateContextAttribsARB`:

```
// определим указатель на функцию создания расширенного контекста OpenGL 3.3
// PFNWGLCREATECONTEXTATTRIBSARBPROC берется из файла wglext.h

PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;

// получим адрес функции создания расширенного контекста OpenGL
wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(
	wglGetProcAddress("wglCreateContextAttribsARB"));

// временный контекст OpenGL нам больше не нужен, удаляем его
wglMakeCurrent(NULL, NULL);
wglDeleteContext(hRCTemp);

// если драйвер видеокарты не предоставил нам адрес этой функции
if (!wglCreateContextAttribsARB)
{
	LOG_ERROR("wglCreateContextAttribsARB fail (%d)\n", GetLastError());
	return false;
}
```

Функция `wglGetProcAddress` очень похожа на стандартную функцию `GetProcAddress`, позволяющую получать указатели на функции из DLL и делает примерно то же самое, т.е. позволяет получить указатель на функцию из драйвера видеокарты, который подгружает и драйвера OpenGL. Если драйвер видеокарты не содержит такой функции - будет возвращен NULL, это может значить что, либо необходимо обновить драйвер видеокарты, либо функция вообще не поддерживается текущей видеокартой и поэтому в драйвере не реализована.

Если все прошло удачно и нам удалось получить адрес функции `wglCreateContextAttribsARB`, то мы можем попробовать создать контекст OpenGL 3.3:

```
// укажем атрибуты для создания расширенного контекста OpenGL 3.3
// атрибуты установлены согласно спецификации расширения:

// WGL_CONTEXT_MAJOR_VERSION_ARB - старшая цифра необходимой версии

// WGL_CONTEXT_MINOR_VERSION_ARB - младшая цифра необходимой версии

// WGL_CONTEXT_FLAGS_ARB - флаги контекста, для нас это контекст с поддержкой
//    нового функционала WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB

// WGL_CONTEXT_PROFILE_MASK_ARB - профиль создаваемого контекста, выбираем
//    WGL_CONTEXT_CORE_PROFILE_BIT_ARB, все устройства с OpenGL 3.2 и старше
//    должны поддерживать профиль CORE, этот профиль позволяет получить доступ
//    к новому функционалу и говорит об отказе от устаревшего функционала

int attribs[] =
{
	WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
	WGL_CONTEXT_MINOR_VERSION_ARB, 3,
	WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
	WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
	0
};

// пробуем создать контекст с поддержкой OpenGL 3.3
hRC = wglCreateContextAttribsARB(hDC, 0, attribs);

// если создать контекст не получилось или он не устанавливается для нашего окна
if (!hRC || !wglMakeCurrent(hDC, hRC))
{
	LOG_ERROR("Creating render context fail (%d)\n", GetLastError());
	return false;
}
```

В итоге мы имеем на руках полнофункциональный контекст с поддержкой OpenGL 3.3, который мы можем использовать для дальнейшей работы.

На этом текущий урок заканчивается, в следующем уроке будет рассмотрен пример загрузки и работы с расширениями OpenGL 3.3.

## Заголовочные файлы ##

Выше я упомянул что описание функции `wglCreateContextAttribsARB` находится в файле [wglext.h](http://www.opengl.org/registry/api/wglext.h) (данный файл можно свободно скачать с сайта opengl.org, по ссылке), также для нормальной работы с расширениями нам необходим будет файл [glext.h](http://www.opengl.org/registry/api/glext.h). Эти файлы содержат форматы, описания и константы расширений и их функций.

## Полезные ссылки ##

  1. [ChoosePixelFormat](http://msdn.microsoft.com/en-us/library/dd318284.aspx)
  1. [SetPixelFormat](http://msdn.microsoft.com/en-us/library/dd369049.aspx)
  1. [wglCreateContextAttribsARB](http://www.opengl.org/registry/specs/ARB/wgl_create_context.txt)
  1. [wglGetProcAddress](http://msdn.microsoft.com/en-us/library/dd374386.aspx)
  1. [wglCreateContext](http://msdn.microsoft.com/en-us/library/dd374379.aspx)

## Исходный код ##

Доступ к исходному коду с проектом для MSVC можно получить двумя способами:
  * Используя SVN [lesson01](https://github.com/glcoder/gl33lessons/tree/lesson01)
  * Скачав архив [lesson01.zip](https://github.com/glcoder/gl33lessons/archive/lesson01.zip) [lesson01.tar.gz](https://github.com/glcoder/gl33lessons/archive/lesson01.tar.gz)
