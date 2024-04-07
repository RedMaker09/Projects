//Manuel Dzimah Castro
#include "gestorPartidas.h"
#include <string>
#include <fstream>
#include <Windows.h>

bool string_es_num_entero_positivo(std::string const& s /*ent*/) {
	
	if (s.size() == 0) return false;

	for (int i = 0; i < s.size(); ++i)
		if (s[i] < -1 || s[i] > 255 || !std::isdigit(s[i])) return false;

	return true;
}

bool leer_movimiento(Juego const& sol /*ent*/, Movimiento &mov /*ent/sal*/) {

		/*Dada una posici�n elegida por el usuario, se averigua las
		distintas posibilidades que se tiene para moverse*/
		sol.posibles_movimientos(mov);

		//Se muestran por pantalla los posibles movimientos a elegir, si los hay
		if (mov.num_dirs() > 1) {
			std::cout << "Selecciona una direccion\n";
			for (int i = 0; i < mov.num_dirs(); ++i)
				std::cout << '\t' << i + 1 << " - " << to_string(mov.direccion(i)) << '\n';

			std::cout << "Respuesta: ";
			std::string dirRespuesta;
			std::getline(std::cin, dirRespuesta);

			while (!string_es_num_entero_positivo(dirRespuesta) || std::stoi(dirRespuesta) > mov.num_dirs() || std::stoi(dirRespuesta) <= 0) {
				std::cout << "Respuesta: ";
				std::getline(std::cin, dirRespuesta);
			}

			mov.fijar_dir_activa(mov.direccion(std::stoi(dirRespuesta) - 1));
			return true;
		}
		else if (mov.num_dirs() == 1) {
			mov.fijar_dir_activa(mov.direccion(0));
			return true;
		}
		else {
			if (sol.posicion_valida(mov.fila(), mov.columna())) std::cout << "La ficha seleccionada no se puede mover.\n";
			else std::cout << "La posicion no es valida o es incorrecta.\n";
			return false;
		}
}

/*Devuelve "true" si se ha salido de una partida pero se desea seguir jugando. 
Si no se desea seguir jugando (hacer "logout"), se devuelve false. Adem�s, por
referencia se obtiene el estado de la partida que se deja, se devuelve "true" si
la partida est� en curso*/
bool simulacion_juego(Juego& sol/*ent/sal*/, bool& enCurso) {

	do {
		std::string f = "", c = "0";

		while (!string_es_num_entero_positivo(f) || !string_es_num_entero_positivo(c)) {
			std::cout << "\nSelecciona la ficha que quieras mover (fila y columna) o escribe \"0 0\" para salir: ";
			std::cin >> f;
			std::cin.get();
			std::getline(std::cin, c);
		}
		if (f == "0" || c == "0") {
			while (f != "S" && f != "N") {
				std::cout << char(168) << "Quieres seguir jugando [S/N]? ";
				std::getline(std::cin, f);
				std::cout << '\n';
			}

			enCurso = true;

			return f == "S";
		}

		Movimiento m(std::stoi(f),std::stoi(c));
		bool movCorrecto = leer_movimiento(sol, m);
		
		if (movCorrecto) {
			sol.jugar(m);
			system("cls"); //Borrar la consola
			sol.mostrar();
		}
	} while (sol.estado() == JUGANDO);

	if (sol.estado() == GANADOR) std::cout  << '\n' << LGREEN << char(173) << "ENHORABUENA! Has ganado :)\n\n";
	else std::cout << '\n' << RED << char(173) << "GAME OVER! Te has quedado bloqueado\n\n";

	std::cout << RESET;

	std::string r = "";
	while (r != "S" && r != "N") {
		std::cout << char(168) << "Quieres seguir jugando [S/N]? ";
		std::getline(std::cin, r);
		std::cout << '\n';
	}

	enCurso = false;

	return r == "S";
}

int main() {

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); //Para ver si hay "memory leaks"

	std::fstream archivo;
	archivo.open("partidas.txt");
	GestorPartidas g;
	g.cargar(archivo);

	std::cout << "Usuario (teclee \"FIN\" para terminar): ";
	std::string loginInfo;
	std::getline(std::cin, loginInfo);

	while (loginInfo != "FIN") {

		g.login(loginInfo);

		int indPart;
		if (g.tiene_partidas()) {
			g.mostrar_partidas();

			std::string res = "";
			while (!string_es_num_entero_positivo(res) || std::stoi(res) > g.num_partidas()) {
				std::cout << "Elige una partida o escribe \"0\" para crear una nueva partida aleatoria: ";
				std::getline(std::cin, res);
				std::cout << '\n';
			}

			if (res == "0") {
				std::string pasos = "";
				while (!string_es_num_entero_positivo(pasos)) {
					std::cout << "Indique el numero de pasos para crear el tablero aleatorio (puedes escribir \"MAX\"): ";
					std::getline(std::cin, pasos);
					if (pasos == "MAX") pasos = "777";
					std::cout << '\n';
				}

				indPart = g.insertar_aleatoria(std::stoi(pasos)) + 1;
			}
			else indPart = std::stoi(res);
		}
		else {
			std::cout << "No tienes partidas. Vamos a crear un juego aleatorio.\n\n";

			std::string pasos = "";
			while (!string_es_num_entero_positivo(pasos)) {
				std::cout << "Indique el numero de pasos para crear el tablero aleatorio (puedes escribir \"MAX\"): ";
				std::getline(std::cin, pasos);
				if (pasos == "MAX") pasos = "777";
				std::cout << '\n';
			}

			indPart = g.insertar_aleatoria(std::stoi(pasos)) + 1;
			
		}

		system("cls"); //Para borrar los tableros que se dan a elegir, si hay
		g.partida(indPart).mostrar();

		bool enCurso;
		bool seguirJugando = simulacion_juego(g.partida(indPart), enCurso);

		system("cls");

		if (!enCurso) g.eliminar_partida(indPart);

		if (!seguirJugando) {
			g.logout();
			std::cout << "LOGGING OUT";
			for (int i = 0; i < 3; ++i) {
				Sleep(400);
				std::cout << '.';
			}
			Sleep(400);
			system("cls");
			std::cout << "Usuario (teclee \"FIN\" para terminar): ";
			std::getline(std::cin, loginInfo);
		}
	}

	//Se guarda en el archivo "partidas.txt" las partidas de cada usuario
	g.salvar(archivo);

	return 0;
}