#include "main.hh"

// SEKCJA: Struktury i Zmienne

//Wiadomości 
enum Tag { REQ, ACC, FIN, NONE };
struct Msg { 
	int timestamp;
	int My_id;
	Tag tag;
	int message;
	};
// Zmienne:
const int N = 8; // ilość procesów
const int P = 5; // ilość pistoletów
const int C = 3; // ilość cykli (na epokę, do podsumowania)
const int A = 3; // ilość kul (amunicji)

// Współdzielone:
int Waiting = -1;

// Prywatne:

vector<int> WaitQueue[0];  // todo jak działały wektory bo mnie pamięć zawodzi XD
int AckNum;
int Partner;
int Wynik_rundy;
int Wynik_calej_rundy;

// Stany i role
enum State {REST, WAIT, INSECTION};
enum Role {KILLER, RUNNER, WAITING};
State myState = REST; 
Role myRole = WAITING;

// Zegary

int max_rank; //czy będą potrzebne?
int my_rank;
int my_time = 0;

#define debug(format, ...) fprintf(stderr, "%4d P%d " format, my_time, my_rank, ##__VA_ARGS__)

void tick(i32 new_time = 0) {
	my_time = max(my_time, new_time) + 1;
}

MPI_Request request = {0};
MPI_Status status = {0};


void Paruj(){
	/*
	wyślij (REQ, time) do wszystkich poza mną
	odbieraj żądania innych dopóki długość WaitQueue jest mniejsza od N
	znajdź siebie w WaitQueue
	if(moja pozycja nieparzysta)
	Wylosuj sobie rolę
	Przekaż przeciwną partnerowi (z pocyji -1)
	else
	czekaj na rolę i na parę
	*/
}

bool try_to_kill(int who){
	//Losuj liczbe
	// Send try-to-kill msg
	// Recv wynik
	bool wynik = false;
	return wynik;
}

int Zbierzwyniki(int Wynik_mojej){

	int suma = 0;
	// while ilość odpowiedzi < ilość procesów - 1
	// recv i dodać do sumy 
	suma += Wynik_mojej;

	return suma;
}






int main(int argc, char **argv) {
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &max_rank);

	debug("my rank %d (max rank %d)\n", my_rank, max_rank);

	for (int iter = 1; true; iter++) {
		// paruj i dobierz role
		Paruj();
		for (int cykl = 1; cykl <= C; cykl++) { // graj C razy
			if(myRole == KILLER){
				int my_ammo = A;
				bool Ofiara_zyje = true;
				bool wynik;
				while(Ofiara_zyje && my_ammo > 0){ 
					wynik = try_to_kill(Partner);
					if(wynik){
						Ofiara_zyje = false;
					}
					my_ammo -= 1;
					// nieudane ++ jesli je liczyc, ale nie ma co
				}
				if(Ofiara_zyje){
					Wynik_rundy += 1;
				}

			}
			else{
				// nic? albo samo słuchanie tutaj? ale każdy powinien słuchać, na wątkach najlepiej
			}
		}
		if(myRole = KILLER){
			cout<<"WYNIK RUNDY W MOJEJ PARZE: "<< Wynik_rundy<<endl;
			// TODO Wynik_calej_rundy = Zbierzwyniki()
			cout<<"WYNIK RUNDY GLOBALNIE: "<<Wynik_calej_rundy<<" na "<<max_rank<<" Zabitych\n";
			// Opcjonalnie mozna zbierac ile wystrzelili, ile prob, zeby tez pokazac - ale nie trzeba
		}

		// No i tak w koło Macieju...
	}

	MPI_Finalize();
	return 0;
}

