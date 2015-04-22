# respak
Resource packer per a JailGames

Ho he fet en Linux, pero deuria funcionar en Mac OSX (o qualsevol sistema operatiu POSIX) sense canvis (no ho he provat).
No funcionará en Windows.

##Per a compilar:
gcc main.c -o respak

##Funcionament:
respak <opció> <arxiu> <directori>

Opcions posibles:
	-p	empaquetar el que hi ha a <directori> i ficar-ho al <arxiu>
	-u	desempaquetar el que hi ha al <arxiu> i ficar-ho al <directori>
	-l	mostrar llista del que hi ha al <arxiu>

La opció per defecte es "-p". El nom del arxiu per defecte es "data.jrf". El
nom del directori per defecte es "data".

Exemples:
	respak	
		empaqueta el que hi ha al directori "./data/" i ho
		fica al arxiu "data.jrf"

	respak -u peric.dat delgat
		desempaqueta el que hi ha al arxiu "peric.dat" i ho
		fica al directori "./delgat/" 

