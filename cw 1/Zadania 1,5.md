## Biblioteka GLM
[OpenGL Mathematics](http://glm.g-truc.net/) (_GLM_) jest biblioteką matematyczną. Posiada to samo nazewnictwo co język shaderów GLSL, dzięki temu zaznajomienie się z jednym pomaga w obsłudze drugiego. Zawiera nie tylko funkcje znane z GLSL, ale jest również rozszerzony o dodatkowe możliwości jak obsługę macierzy przekształceń, kwaternionów, liczb losowych czy szum. 
Dokumentacja znajduje się pod linkiem [https://glm.g-truc.net/0.9.9/api/modules.html](https://glm.g-truc.net/0.9.9/api/modules.html). Ma ona opcję szukania, która pozwala na znalezienie konkretnych funkcji (choć czasem lepiej szukać na tej stronie przez inne wyszukiwarki np google). 
Przykładowo zobaczmy opis tyczący się transformacji, dostępny pod [linkiem](https://glm.g-truc.net/0.9.9/api/a00779.html). Na pierwszy rzut oka funkcje te wyglądają dość enigmatycznie, weźmy funkcję 
```C++
mat< 4, 4, T, Q > translate (vec< 3, T, Q > const &v)
``` 
służy do tworzenia macierzy translacji. Przyjmuje ona jeden argument typu `vec< 3, T, Q >`, który jest wektorem translacji i zwraca zmienną typu `mat< 4, 4, T, Q >`, która jest macierzą translacji. Są to typy szablonowe ([link](https://learn.microsoft.com/pl-pl/cpp/cpp/templates-cpp?view=msvc-170)) jest to metoda programowania generycznego w C++. Parametry szablonu są zapisywane między nawiasami klamrowymi, mogą to byś typy zmiennych lub inne parametry. W przypadku typu `vec` szablon przyjmuje 3 argumenty: pierwszym z nich jest liczba komponentów wektora (w tym przypadku trzy), drugim jest typ danych komponentów a trzecim stopień precyzji, dwa ostatnie są "dowolne".  Natomiast `mat` szablon przyjmuje 4 argumenty: dwa pierwsze określają kształt macierzy, dwa kolejne jak poprzednio określają typ danych i stopień precyzji. Zauważmy też, że `T` i `Q` są takie same w typach zmiennych na wejściu i wyjściu funkcji.

W praktyce `glm` posiada już zdefiniowane typy macierzowe i wektorowe w oparciu te szablony, dzięki nie musimy myśleć co się za nimi kryje będą nas interesować poniższe:
```C++
namespace glm{
...
//wektory
typedef vec< 2, float, defaultp > vec2
typedef vec< 3, float, defaultp > vec3
typedef vec< 4, float, defaultp > vec4
...
//macierze
typedef mat< 2, 2, float, defaultp > 	mat2
typedef mat< 3, 3, float, defaultp > 	mat3 
typedef mat< 4, 4, float, defaultp > 	mat4
...
}
```
Jak zastąpimy generyczne typy z opisu `translate` typami powyżej otrzymamy
```C++
glm::mat4 glm::translate (glm::vec3 const &v)
``` 
Podobnie z funkcjami `rotate` i `scale`. 

### Zadanie
W pliku `ex_1_7.hpp` powtórz zadanie z `ex_1_6.hpp` ale zamiast samodzielnie pisać macierze wykorzystaj funkcje z glm

>**UWAGA** macierze w glm są definiowane kolumnowo, to znaczy **nie dodawaj** operacji transponowania jak to miało miejsce w poprzednich zadaniach. 

## Obsługa wejścia przez GLFW
Za obsługę urządzenia wejścia, jak klawiatura myszka, pady czy joysticki, odpowiada biblioteka GLFW. Jest to opisane w dokumentacji pod linkiem [https://www.glfw.org/docs/latest/group__input.html](https://www.glfw.org/docs/latest/group__input.html)
### Obsługa klawiatury
Klawiaturę obsługujemy w funkcji `processInput`:
```C++
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
	}


```
Aby sprawdzić czy przycisk został wciśnięty musimy odpytać funkcję `glfwGetKey`. Pierwszym argumentem jest okno, natomiast drugim kod przycisku, listę wszystkich kodów można znaleźć pod linkiem [https://www.glfw.org/docs/latest/group__keys.html](https://www.glfw.org/docs/latest/group__keys.html). 

### Zadanie
Zaimplementuj przesuwanie kwadratu za pomocą przycisków strzałek. Zdefiniuj zmienną globalną `glm::vec3 quadPos`, ustaw go jako wektor translacji. Następnie w funkcji `processInput` dodaj zapytania o przyciski strzałek (kody znajdują się poniżej) i przesunięcia w górę, dół, prawo i lewo. 
* `GLFW_KEY_UP`   
* `GLFW_KEY_DOWN`  
* `GLFW_KEY_RIGHT` 
* `GLFW_KEY_LEFT`
### Zadanie*
Zrób dodatkowo obracanie czworościanu w prawo i w lewo wokół własnej osi za pomocą przycisków **O** i **P**.

### Obsługa myszy

Mysz obsługujemy za pomocą *callbacków*, wykorzystamy dwa 
* *cursor position callback* - wywoływana, gdy ruszamy myszką na ekranie funkcja, którą podpinamy to `cursor_position_callback(GLFWwindow* window, double xpos, double ypos)`
* *mouse button callback* - wywoływana, gdy wciskamy przycisk. którą podpinamy to `mouse_button_callback(GLFWwindow* window, int button, int action, int mods)`

Wykorzystanie *callbacków* gwarantuje nam, że operacje będą wykonywane tylko wtedy, gdy tego chcemy. Nie gwarantowałaby nam tego funkcja `processInput`. 

### Zadanie
Zmodyfikuj kod, żeby kwadrat znajdował się w miejscu z myszką z myszką. wewnątrz funkcji `cursor_position_callback` przypisz do współrzędnych x i y `quadPos` wartości zmodyfikowane `xpos` i `ypos`. 
Zmienne `xPos` i `yPos` są opisane w przestrzeni okna. Ich wartość są z przedziałów odpowiednio $[0,\text{width}]$ do $[0,\text{height}]$, gdzie **height**  i **width** to rozmiar ekranu w pikselach, a punkt $(0,0)$ to lewy górny róg ekranu (co w szczególności oznacza, że oś $Y$ jest skierowana w dół). Musimy je przekonwertować do współrzędnych z przestrzeni ekranu, które są z przedziału $[-1,1]$, a oś $Y$ jest skierowana w górę.
Możemy to wykonać za pomocą wzorów.
$$x'=\frac{2x}{width}-1$$
$$y'=-\left(\frac{2y}{height}-1\right)$$
### Zadanie* 
Zakomentuj rozwiązanie poprzedniego zadania. Na bazie jego dodaj funkcjonalność, polegającą na tym, że po wciśnięciu lewego przycisku myszy nowy czworokąt pojawi się w miejscu, w którym kliknęliśmy. Aby to zrobić uzupełnij funkcję `mouse_button_callback`, wykorzystaj `std::vector<glm::vec3> quadsPositions` umieszczaj w niej nowe pozycje kwadratów, następnie narysuj je w pętli wewnątrz `renderScene`. Pamiętaj, żeby w `mouse_button_callback` sprawdzić czy wyciśnięto lewy przycisk, sprawdź czy wartości `button` i `action` są odpowiednie.

## Obsługa pada**

GLFW obsługuje również pady i joysticki, jest możliwość obsługi do 16 urządzeń tego typu. Opisane to zostało w pod linkiem [https://www.glfw.org/docs/latest/input_guide.html#joystick](https://www.glfw.org/docs/latest/input_guide.html#joystick). 

### Zadanie** 
Uzależnij szybkość obrotów czworokąta od stopnia wciśnięcia lewego spustu (*trigger*) pada.
