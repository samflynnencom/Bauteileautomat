Die includes sind in zwei Ordner aufgeteilt:

Im Ordner "driverlib" befinden sich alle Definitionen, die ben�tigt werden, wenn man die TivaWare nutzt. Dort befinden sich auch einige Konstanten. Diese Konstanten ben�tigt man, wenn man eine Applikation entwickelt. Sp�ter kann man ein eigenes Includefile schreiben, in dem die gesamten Konstanten selbst definiert werden k�nnen (muss aber nicht).
----
Im Ordner "hardware" befinden sich alle Includes, die man ben�tigt um die Hardware auf dem Chip selbst anzusprechen. Das bedeutet, wenn man den ADC nutzen m�chte, dann ben�tigt man die Datei hw_adc.h dazu.
---


Normalerweise liefern die Dateien im Ordner "driverlib" die Konstanten wie auch Funktionsprototypen und die im Ordner "hardware" liegenden Includes die Definitionen zur eigentlichen Hardware. Die Tiva CPUs haben allerdings auch die M�glichkeit, die TivaWare, die in ihrem ROM steckt direkt zu nutzen. Daf�r muss man vor alle Funktionen der TivaWare ein "ROM_" setzen und die include "rom.h" aus dem Ordner "driverlib" einbinden. Damit kann man allerdings ausschlie�lich Libraryfunktionen aus dem ROM nutzen. Wenn man sowohl Funktionen im Flash als auch im ROM nutzen m�chte (zum Beispiel beim entwickeln), dann sollte man zus�tzlich noch die Datei "rom_map.h" einbinden, die die Funktionen, die man als ROM_ nutzt, ersatzweise auf die Funktionen im Flash mappt.

Grunds�tzlich gilt: Will man hardware ansprechen, bindet man die entsprechende hw_* aus dem Ordner "hardware" ein und sollte auch die entsprechende Funktion aus der Driverlib einbinden, gegen die man sp�ter linkt.