#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// function prototypes
void divider();
void displayInstructions();
char displayMenu();
int gameMode();
char* choices(int nQuestionIndex, int *nCorrectAnswer);
void singlePlayerMode();
void twoPlayerMode();
int randomizer(int nRandomNumber);
int checkPlayerAnswer(int nCorrectAnswers[], int nPlayerAnswer);
int checkTwoPlayerAnswer(int nCorrectAnswers[], int nPlayer1Answer, int nPlayer2Answer);
int giveScoreTwoPlayers(int nPlayer1Answer, int nPlayer2Answer, int *pPlayer1Score, int *pPlayer2Score, int nPlayerTurn);
int giveScore(int nPlayerAnswer, int *pPlayerScore, int nPlayerTurn);
int awardPoints();
int jackpotRoundAnswers(int nQuestionIndex, const char* cPlayerAnswer);
void jackpotRound();
int giveJackpotScore(int nPlayerAnswer, int *pPlayerScore, int nPlayerTurn);
int giveJackpotWrongScore(int nPlayerAnswer, int *pPlayerScore, int nPlayerTurn);
// portability / input helpers
void clearScreen();
void pauseScreen();
void portableSleep(int nSeconds);
void clearInputBuffer();
int readInt(int *pValue); // 1 = ok, 0 = invalid, -1 = end-of-file
int isInputClosed();

// Set when stdin reaches end-of-file (e.g. piped input exhausted or Ctrl+D/Ctrl+Z).
// Used to unwind back to the menu and quit instead of spinning on invalid input.
static int g_inputClosed = 0;

int main() {
	srand((unsigned int) time(NULL));
	char cChoice;
	int nGameModeChoice;
	int nGameEnd = 0;
	int nProgEnd = 0;

		do {
			clearScreen(); // clears anything showed to always show the menu alone
			cChoice = displayMenu(); // calls the player's choice of destination (PLAY, HOW TO PLAY, OR QUIT)

			switch (cChoice) {
				case 'H':
				case 'h':
				clearScreen();	// clears the main menu to show the instructions
				displayInstructions(); // displays the instructions of the game
				pauseScreen(); // pauses the program
				break;
				case 'P':
				case 'p':

					nGameEnd = 0; // FIX: reset so re-entering Play mode re-prompts correctly on invalid input
					do {

					clearScreen(); // clears the screen
					nGameModeChoice = gameMode(); // this means that the parameter (nGameModeChoice) is from the function gameMode()

						switch (nGameModeChoice) {

							case 1: // for single player mode
							clearScreen(); // clears the previous screen
							singlePlayerMode();
							portableSleep (5);
							clearScreen(); // for ending the loop
							nGameEnd = 1;
							break;

							case 2: // for two player mode
							clearScreen(); // clears the previous screen
							twoPlayerMode();
							if (!isInputClosed()) {
								jackpotRound();
							}
							portableSleep (5);
							clearScreen(); // for ending the loop
							nGameEnd = 1;
							break;
							case 3: // for returning to the main menu
							nGameEnd = 1;
							break;

							default:
							clearScreen();  // clears the previous screen
							printf ("Invalid option. Please try again.\a\n\n");

							pauseScreen(); // pauses the program
						}

					} while (!nGameEnd);
					break;
				case 'Q':
				case 'q':
				clearScreen(); // clears the main menu to show exit page
				printf ("\nThank you for playing family feud!\n");
				nProgEnd = 1;  // for ending the loop
				break;
				default:
				clearScreen(); // clears the main menu to show the main menu again
				printf ("Invalid option. Please try again.\a\n\n");
				pauseScreen(); // pauses the program
				break;
			}
	} while (!nProgEnd); // ensures that the loop will never end unless nLoopEnd becomes 0
	return 0;
}

/*
Descripton:		this function is a divider
*/
void divider() {
	printf ("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
}

/*
Descripton:		this function shows the instructions of the game
*/
void displayInstructions() {
	printf ("\n%35s", "- - - HOW TO PLAY FAMILY FEUD - - -\n");
	printf ("\n1. The game consists of 5 questions in the main round.\n");
	printf ("2. Players take turns, but after every incorrect answer, the turn switches to the other player.\n");
	printf ("3. Each question has 4 possible answers, and each answer has points associated with it.\n");
	printf ("4. The player with the highest score after 5 questions will move to the jackpot round.\n");
	printf ("5. In the jackpot round, the winner will answer 5 random questions and must score 200 points to win the jackpot.\n");
	divider();
}

/*
Description:	This function displays the main menu of the game as well as asks the users whether they want to play, show the instructions, or quit
Precondition:	cChoice is are characters "H", "P" and "Q"
				Other characters and floating point characters are not accepted as valid inputs

@return - 		cChoice for the user's selected destination in the program (PLAY, HOW TO PLAY, OR QUIT)
*/
char displayMenu() {
	char cChoice = '\0';

	printf ("\n%25s", "FAMILY FEUD GAME\n");
	printf ("%24s\n", "By: Gian Lawan\n");
	printf ("%29s\n", "- - - MAIN MENU - - - \n");
	printf ("%24s", "[H] How to Play\n");
	printf ("%22s", "[P] Play Game\n");
	printf ("%22s", "[Q] Quit Game\n");
	divider();
	printf ("\nEnter your choice (H, P, Q): ");
	fflush(stdout);
	{
		int nResult = scanf (" %c", &cChoice);
		if (nResult == EOF) {
			g_inputClosed = 1; // FIX: treat closed input as a quit request instead of looping forever
			return 'Q';
		}
		if (nResult != 1) {
			clearInputBuffer();
			return '\0';
		}
	}
	clearInputBuffer();

	return cChoice; // stores the player's choice
}

/*
Description:	This function displays what game mode should be played and would store the user's choice of gamemode. (singleplayer or two-player)
Precondition:	nGameMode is a non-negative integer from 1-3
				Other characters and floating point characters are not accepted as valid inputs

@return - 		nGameMode for the user's selected gamemode (1 or 2 for singleplayer or two-player respectively, 3 for returning to the main menu)
				returns -1 if the input is not a valid integer
*/
int gameMode() {
	int nGameModeChoice = -1;
	int nReadResult;

	printf ("- - - CHOOSE YOUR PREFFERED MODE TO BEGIN THE GAME - - -\n\n");
	printf ("%32s","[1] - One Player\n");
	printf ("%33s","[2] - Two Players\n");
	printf ("%41s","[3] - Return to main menu\n");
	divider();
	printf ("\nSelect an option (1, 2, 3): ");
	fflush(stdout);
	nReadResult = readInt(&nGameModeChoice);
	if (nReadResult == -1) {
		return 3; // FIX: closed input goes back to the menu (which then quits) instead of looping
	}
	if (nReadResult == 0) {
		return -1; // FIX: signal invalid (non-numeric) input instead of returning garbage
	}

	return nGameModeChoice; // stores the player's gamemode choice

}

/*
Description: 	This functions prints the choices for the corresponding question
	@param - 	nQuestionIndex is the randomized number to determine the randomly selected quetion

Precondition: 	nQuestionIndex is a non-negative integer
				i is a non-negative integer
				j is a non-negative integer
				nRandomAnswer is a randomly generated non-negative integer based on the questions range of answers
				nUniqueAnswer is either true or false (1 or 0)
				nRandomChoices stores the randomly generated answers for the options
				nCorrectAnswer stores the randomly generated answers for the options
				Other characters and floating point characters are not accepted as valid inputs

@return -		cChoices is the string to give the randomly generated options for the question
				*nCorrectAnswer is the array that stores the randomly generated answers for the options
*/
char* choices(int nQuestionIndex, int *nCorrectAnswer) {
	int i, j;
    static char cChoices[64]; // buffer to hold the generated string

    if (nQuestionIndex == 0 || nQuestionIndex == 1 || nQuestionIndex == 8 || nQuestionIndex == 12) { // for questions 0, 1, 8, 12 which requires a certain set of range of values
        int nRandomChoices[4]; // 4, since we have 4 options

       			for (i = 0; i < 4; i++) { // this loop is for generating a random answer 4 times
        			int nRandomAnswer;
        			int nUniqueAnswer;

        			do { // this loop generates the value of the answer
					nRandomAnswer = 30 + rand () % 31; // randomly generates a number from 30-60
					nUniqueAnswer = 1; // nUniqueAnswer is set to TRUE

						for (j = 0; j < i; j++) { // this loop checks if the randomly generated value has been repeated
						nUniqueAnswer &= (nRandomChoices[j] != nRandomAnswer); // this expression checks if the randomly generated value has been repeated, if not nUniqueAnswer stores the value TRUE
																				   // if the randomly generated value has been repeated, nUniqueAnswer stores the value FALSE
						}
					} while (!nUniqueAnswer); // if nUniqueAnswer stores the value TRUE, it will be FALSE which ends the do-while loop generating the randomly generated values. If nUniqueAnswer stores the value FALSE, then it repeats the process
        			nRandomChoices[i] = nRandomAnswer; // this expression stores the randomly generated values
					nCorrectAnswer[i] = nRandomAnswer; // this expression also stores the randomly generated values
        }
        snprintf(cChoices, sizeof(cChoices), "\t\t%d\n\t\t%d\n\t\t%d\n\t\t%d", nRandomChoices[0], nRandomChoices[1], nRandomChoices[2], nRandomChoices[3]); // this expression prints all the choices

    } else if (nQuestionIndex == 2 || nQuestionIndex == 7 || nQuestionIndex == 10 || nQuestionIndex == 11) { // for questions 2, 7, 10, 11 which requires a certain set of range of values
     	int nRandomChoices[4]; // 4, since we have 4 options

       			for (i = 0; i < 4; i++) { // this loop is for generating a random answer 4 times
        			int nRandomAnswer;
        			int nUniqueAnswer;

        			do { // this loop generates the value of the answer
					nRandomAnswer = 1 + rand () % 10; // randomly generates a number from 1-10
					nUniqueAnswer = 1; // nUniqueAnswer is set to TRUE

						for (j = 0; j < i; j++) { // this loop checks if the randomly generated value has been repeated
						nUniqueAnswer &= (nRandomChoices[j] != nRandomAnswer); // this expression checks if the randomly generated value has been repeated, if not nUniqueAnswer stores the value TRUE
																				   // if the randomly generated value has been repeated, nUniqueAnswer stores the value FALSE
						}
					} while (!nUniqueAnswer); // if nUniqueAnswer stores the value TRUE, it will be FALSE which ends the do-while loop generating the randomly generated values. If nUniqueAnswer stores the value FALSE, then it repeats the process
        			nRandomChoices[i] = nRandomAnswer; // this expression stores the randomly generated values
        			nCorrectAnswer[i] = nRandomAnswer; // this expression also stores the randomly generated values
        }
        snprintf(cChoices, sizeof(cChoices), "\t\t%d\n\t\t%d\n\t\t%d\n\t\t%d", nRandomChoices[0], nRandomChoices[1], nRandomChoices[2], nRandomChoices[3]); // this expression prints all the choices

    } else if (nQuestionIndex == 3 || nQuestionIndex == 5 || nQuestionIndex == 6) { // for questions 3, 5, 6 which requires a certain set of range of values
        int nRandomChoices[4]; // 4, since we have 4 options

       			for (i = 0; i < 4; i++) { // this loop is for generating a random answer 4 times
					int nRandomAnswer;
        			int nUniqueAnswer;

        			do { // this loop generates the value of the answer
					nRandomAnswer = 1 + rand () % 5; // randomly generates a number from 1-5
					nUniqueAnswer = 1; // nUniqueAnswer is set to TRUE

						for (j = 0; j < i; j++) { // this loop checks if the randomly generated value has been repeated
						nUniqueAnswer &= (nRandomChoices[j] != nRandomAnswer); // this expression checks if the randomly generated value has been repeated, if not nUniqueAnswer stores the value TRUE
																				   // if the randomly generated value has been repeated, nUniqueAnswer stores the value FALSE

						}
					} while (!nUniqueAnswer); // if nUniqueAnswer stores the value TRUE, it will be FALSE which ends the do-while loop generating the randomly generated values. If nUniqueAnswer stores the value FALSE, then it repeats the process
        			nRandomChoices[i] = nRandomAnswer; // this expression stores the randomly generated values
        			nCorrectAnswer[i] = nRandomAnswer; // this expression also stores the randomly generated values
        }
        snprintf(cChoices, sizeof(cChoices), "\t\t%d\n\t\t%d\n\t\t%d\n\t\t%d", nRandomChoices[0], nRandomChoices[1], nRandomChoices[2], nRandomChoices[3]); // this expression prints all the choices

    } else if (nQuestionIndex == 4 || nQuestionIndex == 9 || nQuestionIndex == 13 || nQuestionIndex == 14) { // for questions 4, 9, 13, 14 which requires a certain set of range of values
        int nRandomChoices[4]; // 4, since we have 4 options

       			for (i = 0; i < 4; i++) { // this loop is for generating a random answer 4 times
        			int nRandomAnswer;
        			int nUniqueAnswer;

        			do { // this loop generates the value of the answer
					nRandomAnswer = 15 + rand () % 11; // randomly generates a number from 15-25
					nUniqueAnswer = 1; // nUniqueAnswer is set to TRUE

						for (j = 0; j < i; j++) { // this loop checks if the randomly generated value has been repeated
						nUniqueAnswer &= (nRandomChoices[j] != nRandomAnswer); // this expression checks if the randomly generated value has been repeated, if not nUniqueAnswer stores the value TRUE
																				   // if the randomly generated value has been repeated, nUniqueAnswer stores the value FALSE

						}
					} while (!nUniqueAnswer); // if nUniqueAnswer stores the value TRUE, it will be FALSE which ends the do-while loop generating the randomly generated values. If nUniqueAnswer stores the value FALSE, then it repeats the process
        			nRandomChoices[i] = nRandomAnswer; // this expression stores the randomly generated values
        			nCorrectAnswer[i] = nRandomAnswer; // this expression also stores the randomly generated values
        }
        snprintf(cChoices, sizeof(cChoices), "\t\t%d\n\t\t%d\n\t\t%d\n\t\t%d", nRandomChoices[0], nRandomChoices[1], nRandomChoices[2], nRandomChoices[3]); // this expression prints all the choices
    } else {
		// FIX: defensive fallback for an unexpected question index
		for (i = 0; i < 4; i++) {
			nCorrectAnswer[i] = i + 1;
		}
		snprintf(cChoices, sizeof(cChoices), "\t\t%d\n\t\t%d\n\t\t%d\n\t\t%d", 1, 2, 3, 4);
	}
    return cChoices; // returns the string
}

/*
Description: 	This function displays the randomly generated questions, the choices, and the answer bar for singleplayer mode

Precondition: 	Other characters and floating point characters are not accepted as valid inputs
*/
void singlePlayerMode() {
	const char *cQuestions[15] = {
	"How many hours does an average person spend on social media in a week?",
	"How many hours does an average person spend on a phone in a week?",
	"How many hours does an average person get when they are taking a nap in a day?",
	"How many pets does an average person have?",
	"What age does a person get their first relationship?",
	"How many games does a person have in their cellphone?",
	"How many toys does an average kid have?",
	"How many times does a person seen their favorite movie?",
	"What is the average number of hours you spend playing video games each week?",
	"How many times does a person listened to their favorite song?",
	"What do you think is the total number of pets a person had in their life?",
	"How many hours does an average person workout?",
	"How long do you stay in the bathroom (in minutes)?",
	"How many hours does a student spend on homework on a week?",
	"How much does a person spend (PHP, in thousands) when they are treating 15 people?"};
	int nTotalQuestions = 15;
	int nAsked[15] = {0}; // 0 means the question is unasked, so it sets all the questions unasked
	int nNumAsked;
	int nRandomQuestion;
	int nRandomNumber;
	int nIsValid = 1;
	int pPlayerScore = 0;
    int nPlayerTurn = 1;

		for (nNumAsked = 0;nNumAsked < 5;) { // loops for up to 5 questions
        nRandomNumber = rand() % nTotalQuestions; // selects a random number from 0-14
        nRandomQuestion = randomizer(nRandomNumber); // the random number gets randomized more

        if (nAsked[nRandomQuestion] == 0) { // checks if the question has been unasked
            int nPlayerAnswer = 0;
            nAsked[nRandomQuestion] = 1; // if the question is unasked, this line stores the question as asked and ensures that it shouldn't repeat
            nNumAsked++; // increments if the question has been marked as asked
            printf ("Loading...\n");
            fflush(stdout);
            portableSleep (3);
            clearScreen(); // clears the loading screen
            printf ("%34s","- - - ONE PLAYER - - -\n\n");
            printf ("%31s","YOUR QUESTION IS\n\n");
            printf ("%s\n\n", cQuestions[nRandomQuestion]); // shows the randomly generated question (not repeated)

            int nCorrectAnswers[4]; // the array to hold the correct answers
            // FIX: call choices() only once (previous code called it twice, discarding the first set)
            {
                char *pChoices = choices(nRandomQuestion, nCorrectAnswers); // passes the nCorrectAnswers array
                printf ("%s\n\n", pChoices); // shows the question's corresponding choices
            }

            	if (nPlayerTurn == 1) {
            		do {
            			int nReadResult;
            			printf("Enter your answer: ");
            			fflush(stdout);
            			nReadResult = readInt(&nPlayerAnswer);
            			if (nReadResult == -1) { // FIX: closed input ends the game instead of looping forever
            				printf ("\nInput closed. Ending the game...\n");
            				return;
            			}
            			if (nReadResult == 0) { // FIX: handle non-numeric input safely
            				printf ("Invalid input. Please enter a number from the choices.\a\n\n");
            				nIsValid = 0;
            				continue;
            			}

            			if (checkPlayerAnswer(nCorrectAnswers, nPlayerAnswer)) { // the player's answer get validated
           				printf ("Invalid answer, select again\a\n\n");
           				nIsValid = 0; // if the answer/s is/are not valid then it loop again
					   } else {
						printf ("\n");
            		  	nIsValid = 1; // if the answer/s is/are valid then it would end the loop
            		   }
					} while (!nIsValid);

				}
            // FIX: score only when a new question was actually asked (was outside the if-block,
            // scoring uninitialized answers on repeated questions)
    		nPlayerTurn = giveScore(nPlayerAnswer, &pPlayerScore, nPlayerTurn);
    	}
	}
	divider();
	printf ("\nFINAL SCORE:\n"); // displays final scores
	printf("Your score is: %d points\n", pPlayerScore);

		if (pPlayerScore >= 150) { // determines if the player gets to advance
			printf("Congratulations you advance to the jackpot round!\n\n");
			pauseScreen();
			jackpotRound();
		} else if (pPlayerScore < 150) {
			printf("Unfortunately you need %d more point/s to advance to the jackpot round.\n\n", 150 - pPlayerScore);
	}
}

/*
Description: 	This function displays the randomly generated questions, the choices, and the answer bar for two player mode

Precondition: 	Other characters and floating point characters are not accepted as valid inputs
*/
void twoPlayerMode() {
	const char *cQuestions[15] = {
	"How many hours does an average person spend on social media in a week?",
	"How many hours does an average person spend on a phone in a week?",
	"How many hours does an average person get when they are taking a nap in a day?",
	"How many pets does an average person have?",
	"What age does a person get their first relationship?",
	"How many games does a person have in their cellphone?",
	"How many toys does an average kid have?",
	"How many times does a person seen their favorite movie?",
	"What is the average number of hours you spend playing video games each week?",
	"How many times does a person listened to their favorite song?",
	"What do you think is the total number of pets a person had in their life?",
	"How many hours does an average person workout?",
	"How long do you stay in the bathroom (in minutes)?",
	"How many hours does a student spend on homework on a week?",
	"How much does a person spend (PHP, in thousands) when they are treating 15 people?"};
	int nTotalQuestions = 15;
	int nAsked[15] = {0}; // 0 means the question is unasked, so it sets all the questions unasked
	int nNumAsked;
	int nRandomQuestion;
	int nRandomNumber;
	int nIsValid = 1;
	int pPlayer1Score = 0;
    int pPlayer2Score = 0;
    int nPlayerTurn = 1;

		for (nNumAsked = 0;nNumAsked < 5;) { // loops for up to 5 questions
        nRandomNumber = rand() % nTotalQuestions; // selects a random number from 0-14
        nRandomQuestion = randomizer(nRandomNumber); // the random number gets randomized more

        if (nAsked[nRandomQuestion] == 0) { // checks if the question has been unasked
            int nPlayer1Answer = 0, nPlayer2Answer = 0;
            nAsked[nRandomQuestion] = 1; // if the question is unasked, this line stores the question as asked and ensures that it shouldn't repeat
            nNumAsked++; // increments if the question has been marked as asked
            printf ("Loading...\n");
            fflush(stdout);
			portableSleep (4);
            clearScreen(); // clears the loading screen
            printf ("%34s","- - - TWO PLAYER - - -\n\n");
            printf ("%31s","YOUR QUESTION IS\n\n");
            printf ("%s\n\n", cQuestions[nRandomQuestion]); // shows the randomly generated question (not repeated)

            int nCorrectAnswers[4]; // the array to hold the correct answers
            // FIX: call choices() only once (previous code called it twice, discarding the first set)
            {
                char *pChoices = choices(nRandomQuestion, nCorrectAnswers); // passes the nCorrectAnswers array
                printf ("%s\n\n", pChoices); // shows the question's corresponding choices
            }

          		if (nPlayerTurn == 1) { // player 1 goes first
            		do {
            			int nReadResult;
            			printf("Enter Player 1 answer: ");
            			fflush(stdout);
            			nReadResult = readInt(&nPlayer1Answer); // gets player 1's answer
            			if (nReadResult == -1) { // FIX: closed input ends the game instead of looping forever
            				printf ("\nInput closed. Ending the game...\n");
            				return;
            			}
            			if (nReadResult == 0) {
            				printf ("Invalid input. Please enter a number from the choices.\a\n\n");
            				nIsValid = 0;
            				continue;
            			}

            				if (checkPlayerAnswer(nCorrectAnswers, nPlayer1Answer)) { // player 1's answer gets validated
           				printf ("Invalid answer, either not in the choices or same answer with the previous player, select again\a\n\n");
           				nIsValid = 0; // if the answer/s is/are not valid then it loop again
							} else {
							printf ("\n");
            	   		nIsValid = 1; // if the answer/s is/are valid then it would end the loop
            	   	}
					} while (!nIsValid);

					do {
            			int nReadResult;
            			printf("Enter Player 2 answer: ");
            			fflush(stdout);
            			nReadResult = readInt(&nPlayer2Answer); // gets player 2's answer
            			if (nReadResult == -1) { // FIX: closed input ends the game instead of looping forever
            				printf ("\nInput closed. Ending the game...\n");
            				return;
            			}
            			if (nReadResult == 0) {
            				printf ("Invalid input. Please enter a number from the choices.\a\n\n");
            				nIsValid = 0;
            				continue;
            			}

           					if (checkTwoPlayerAnswer(nCorrectAnswers, nPlayer1Answer, nPlayer2Answer)) { // player 2's answer gets validated while ensuring it also isn't equal with player 1's answer
           					printf ("Invalid answer, either not in the choices or same answer with the previous player, select again\a\n\n");
           					nIsValid = 0; // if the answer/s is/are not valid then it loop again
							} else {
							printf ("\n");
            	   		nIsValid = 1; // if the answer/s is/are valid then it would end the loop
            				}
        			} while (!nIsValid);

    			} else if (nPlayerTurn == 2){ // player 2 goes first
    				do {
            			int nReadResult;
            			printf("Enter Player 2 answer: ");
            			fflush(stdout);
            			nReadResult = readInt(&nPlayer2Answer); // gets player 2's answer
            			if (nReadResult == -1) { // FIX: closed input ends the game instead of looping forever
            				printf ("\nInput closed. Ending the game...\n");
            				return;
            			}
            			if (nReadResult == 0) {
            				printf ("Invalid input. Please enter a number from the choices.\a\n\n");
            				nIsValid = 0;
            				continue;
            			}

            				if (checkPlayerAnswer(nCorrectAnswers, nPlayer2Answer)) { // player 2's answer gets validated
           					printf ("Invalid answer, either not in the choices or same answer with the previous player, select again\a\n\n");
           					nIsValid = 0; // if the answer/s is/are not valid then it loop again
							} else {
							printf ("\n");
            	   		nIsValid = 1; // if the answer/s is/are valid then it would end the loop
            	   	}
					} while (!nIsValid);

					do {
            			int nReadResult;
            			printf("Enter Player 1 answer: ");
            			fflush(stdout);
            			nReadResult = readInt(&nPlayer1Answer); // gets player 1's answer
            			if (nReadResult == -1) { // FIX: closed input ends the game instead of looping forever
            				printf ("\nInput closed. Ending the game...\n");
            				return;
            			}
            			if (nReadResult == 0) {
            				printf ("Invalid input. Please enter a number from the choices.\a\n\n");
            				nIsValid = 0;
            				continue;
            			}

           					if (checkTwoPlayerAnswer(nCorrectAnswers, nPlayer2Answer, nPlayer1Answer)) { // player 1's answer gets validated while ensuring it also isn't equal with player 2's answer
           					printf ("Invalid answer, either not in the choices or same answer with the previous player, select again\a\n\n");
           					nIsValid = 0; // if the answer/s is/are not valid then it loop again
							} else {
							printf ("\n");
            	   		nIsValid = 1; // if the answer/s is/are valid then it would end the loop
            				}
        			} while (!nIsValid);
				}
    		// FIX: score only when a new question was actually asked (was outside the if-block,
    		// scoring uninitialized answers on repeated questions)
    		nPlayerTurn = giveScoreTwoPlayers(nPlayer1Answer, nPlayer2Answer, &pPlayer1Score, &pPlayer2Score, nPlayerTurn); // determines who gets the turn next, the points gained in 1 round, and the total score
    	}
	}
	divider();
	printf ("\nFINAL SCORE:\n"); // displays final scores
	printf("Player 1: %d points\n", pPlayer1Score);
	printf("Player 2: %d points\n\n", pPlayer2Score);

		if (pPlayer1Score > pPlayer2Score) { // determines the winner
			printf("Player 1 is the winner and advances to the jackpot round!\n\n");
			pauseScreen();
		} else if (pPlayer2Score > pPlayer1Score) {
			printf("Player 2 is the winner and advances to the jackpot round!\n\n");
			pauseScreen();
		} else {
			printf ("Tie!\n");
			printf("Flipping a coin to determine the winner. . .\n\n");
			fflush(stdout);
			int i = (rand() % 2) + 1;
			portableSleep (3);
			switch (i) {
				case 1:
				printf("Player 1 wins by a coin flip and advances to the jackpot round!\n\n");
				pauseScreen();
				break;
				case 2:
				printf("Player 2 wins by a coin flip and advances to the jackpot round!\n\n");
				pauseScreen();
				break;
				default: // FIX: defensive default (unreachable, silences warnings)
				printf("Player 1 wins by a coin flip and advances to the jackpot round!\n\n");
				pauseScreen();
				break;
			}
		}
}

/*
Description:	This function further randomizes the question chose in the function questionAndAnswer
	@param - 	nRandomNumber - random number generated in the questionAndAnswer function

Precondition: 	nRandomizer is a non-negative integer from 1-4
				nDivisionCase is a non-negative integer from 5-6

@return - 		nRandomNumber	- randomized non-negative integer from 0-14 for the questions
*/
int randomizer(int nRandomNumber) {
	int nRandomizer;
	nRandomizer = ((rand() % 3) + 1); // randomly generates a number from 1-3

		switch (nRandomizer) { // switch case because its faster than if-else
			case 1: // if the randomly generated number is 1, it goes to an addition case, and mod case
			return ((nRandomNumber + 7) % 15); // these randomly generated expressions ensure that the result will always be 0-14, then returns it to the questionAndAnswer function

			case 2: // if the randomly generated number is 2, it goes to a subtraction case, addition case, and mod case
			return (((nRandomNumber - 7) + 15) % 15); // these randomly generated expressions ensure that the result will always be 0-14, then returns it to the questionAndAnswer function

			case 3: // if the randomly generated number is 3, it goes to a multiplication case, and mod case
			return ((nRandomNumber * 2) % 15); // these randomly generated expressions ensure that the result will always be 0-14, then returns it to the questionAndAnswer function

			default: // FIX: defensive default (unreachable since nRandomizer is 1-3)
			return (nRandomNumber % 15);
		}
}

/*
Description:	This function checks if the player inputted an answer within the choices
	@param - 	nCorrectAnswers - is the array that stored the randomly generated answers for the options
				nPlayerAnswer - is the player's answer

Precondition: 	nPlayerAnswer is a non-negative integer and only accepts numbers that are in the nCorrectAnswer array
				nCorrectAnswers is an array that stored the randomly generated answers

@return - 		0 if true, otherwise 1
*/
int checkPlayerAnswer(int nCorrectAnswers[], int nPlayerAnswer) {

		if (nCorrectAnswers[0] == nPlayerAnswer || nCorrectAnswers[1] == nPlayerAnswer || nCorrectAnswers[2] == nPlayerAnswer || nCorrectAnswers[3] == nPlayerAnswer) { // this expressions check if the player inputted a number in the array
			return 0;
		} else {
			return 1;
		}
}

/*
Description:	This function checks if the players inputted an answer within the choices
	@param - 	nCorrectAnswers - is the array that stored the randomly generated answers for the options
				nPlayer1Answer - is the player 1's answer
				nPlayer2Answer - is the player 2's answer

Precondition: 	nPlayer1Answer is a non-negative integer and only accepts numbers that are in the nCorrectAnswer array
				nPlayer2Answer is a non-negative integer and only accepts numbers that are in the nCorrectAnswer array
				nCorrectAnswers is an array that stored the randomly generated answers
				nPlayer1Answer and nPlayer2Answer is should not be equal

@return - 		0 if true, otherwise 1
*/
int checkTwoPlayerAnswer(int nCorrectAnswers[], int nPlayer1Answer, int nPlayer2Answer) {

		if ((nPlayer1Answer != nPlayer2Answer) && (nCorrectAnswers[0] == nPlayer2Answer || nCorrectAnswers[1] == nPlayer2Answer || nCorrectAnswers[2] == nPlayer2Answer || nCorrectAnswers[3] == nPlayer2Answer) && (nCorrectAnswers[0] == nPlayer1Answer || nCorrectAnswers[1] == nPlayer1Answer || nCorrectAnswers[2] == nPlayer1Answer || nCorrectAnswers[3] == nPlayer1Answer)) { // this expressions check if the player inputted a number in the array, and also check if player 1 and player 2's answers are different
			return 0;
		} else {
			return 1;
		}
}

/*
Description:	This function determines the round winner, and determines who to gets to guess first for two players
	@param - 	nPlayer1Answer - is the player 1's answer
				nPlayer2Answer - is the player 2's answer
				*pPlayer1Score - is the player 1's score
				*pPlayer2Score - is the player 2's score
				nPlayerTurn - determines who gets to guess first

Precondition: 	nPlayer1Points, nPlayer2Points, *pPlayer1Score, *pPlayer2Score, nPlayerTurn are non-negative integers
				This function only works when nPlayer1Answer != nPlayer2Answer



@return - 		1 if player 1 gets to guess first, 2 if player 2 gets to guess first
*/
int giveScoreTwoPlayers(int nPlayer1Answer, int nPlayer2Answer, int *pPlayer1Score, int *pPlayer2Score, int nPlayerTurn) {
    int nCurrentRoundWinner = 0;
    int nPlayer1Points = 0, nPlayer2Points = 0; // FIX: initialize to avoid use-of-uninitialized when answers are equal

    		if (nPlayer1Answer != nPlayer2Answer) { // this expression checks if the user placed an answer
    		do { // this loop ensures that the points shoulnd't be tied
    		nPlayer1Points = awardPoints(); // generates points for player 1
    		nPlayer2Points = awardPoints(); // generates points for player 2
    		} while (nPlayer1Points == nPlayer2Points);

       	 		*pPlayer1Score += nPlayer1Points; // stores the points for player 1
       	 		*pPlayer2Score += nPlayer2Points; // stores the points for player 2
				if (nPlayer1Points > nPlayer2Points) { // determines who to guesses first next round, the person with the most points in that round guesses first
				nCurrentRoundWinner = 1;
    		} else if (nPlayer2Points > nPlayer1Points) {
        		nCurrentRoundWinner = 2;
    		}
		}

	clearScreen(); // clears the screen to display the points
	divider();
	printf("\nPoints gained this round:\n"); // displays final scores for the round
    printf("Player 1: %d point/s\n", nPlayer1Points);
    printf("Player 2: %d point/s\n", nPlayer2Points);
	divider();
	divider();
	printf("\nTotal score after this round:\n"); // displays final scores for the round
    printf("Player 1: %d point/s\n", *pPlayer1Score);
    printf("Player 2: %d point/s\n", *pPlayer2Score);
    divider();

    	if (nCurrentRoundWinner == 1) { // this expression passes who determines who to guess first next round
        	return 1;
    	} else if (nCurrentRoundWinner == 2) {
        	return 2;
    	}
    return nPlayerTurn;
}

/*
Description:	This function determines the score given for singleplayer mode
	@param - 	nPlayerAnswer - is the player's answer
				*pPlayerScore - is the player's score
				nPlayerTurn - is to give the turn to the player again

Precondition: 	nPlayerPoints, *pPlayer1Score, nPlayerTurn are non-negative integers



@return - 		nPlayerTurn - the player gets the turn again
*/
int giveScore(int nPlayerAnswer, int *pPlayerScore, int nPlayerTurn) {
    int nCurrentRoundWinner = 0;
    int nPlayerPoints = 0; // FIX: initialize to avoid use-of-uninitialized

    		if (nPlayerAnswer > -1) { // this expression checks if the user placed an answer
    		nPlayerPoints = awardPoints(); // generates points
       	 		*pPlayerScore += nPlayerPoints; // stores the points for the player
				nCurrentRoundWinner = 1;
		}

	clearScreen(); // clears the screen to display the points
	divider();
	printf("\nPoint/s gained this round:\n"); // displays final scores for the round
    printf("You gained: %d point/s\n", nPlayerPoints);
	divider();
	divider();
	printf("\nTotal score after this round:\n"); // displays final scores for the round
    printf("Your score: %d point/s\n", *pPlayerScore);
    divider();

    	if (nCurrentRoundWinner == 1) { // this expression that the player gets to the turn again
        	return 1;
    	}
    return nPlayerTurn;
}

/*
Description:	This function generate points to the user
Precondition: 	The generated number should be 0-100 only

@return - 		nPoints - Numbers 0-100 only
*/
int awardPoints() {
	int nPoints = 0, nAnswer;

	nAnswer = rand() % 4 + 1; // this selects a number from 1-4
	switch (nAnswer) {
		case 1:
		nPoints = 0; // FIX: simplified from ((rand() % 100) * 0); points is 0, which indicates a wrong answer
		break;
		case 2:
		case 3:
		case 4:
		nPoints = ((rand() % 100) + 1); // points are generated randomly from 1 to 100
		break;
		default: // FIX: defensive default (unreachable)
		nPoints = 0;
		break;
	}
	return nPoints;
}

/*
Description:	This function determines whether the player's string input in the jackpot round is valid
	@param - 	nQuestionIndex is the randomized number to determine the randomly selected quetion
				cPlayerAnswer is the player's string input

Precondition:  	nQuestionIndex is a non-negative integer
				cPlayerAnswer only accepts string inputs (normalized to ALL CAPS before comparison)

@return - 		1 if the answer is valid, 0 otherwise
*/
int jackpotRoundAnswers(int nQuestionIndex, const char* cPlayerAnswer) {
	// FIX: return int (1 = valid, 0 = invalid) instead of NULL vs string pointer,
	// which was confusing and inverted. Also made input const-correct.
	if (cPlayerAnswer == NULL) {
		return 0;
	}
	// this expression checks the player's string input and contains all the valid answers for the jackpot rounds
	if (nQuestionIndex == 0) {
		if (strcmp(cPlayerAnswer, "LEGS") == 0 || strcmp(cPlayerAnswer, "LIPS") == 0 || strcmp(cPlayerAnswer, "LUNGS") == 0 || strcmp(cPlayerAnswer, "LIVER") == 0)
		{
			return 1;
		}
	} else if (nQuestionIndex == 1) {
		if (strcmp(cPlayerAnswer, "GUITAR") == 0 || strcmp(cPlayerAnswer, "BANJO") == 0 || strcmp(cPlayerAnswer, "UKULELE") == 0 || strcmp(cPlayerAnswer, "HARP") == 0)
		{
			return 1;
		}
	} else if (nQuestionIndex == 2) {
		if (strcmp(cPlayerAnswer, "DIVER") == 0 || strcmp(cPlayerAnswer, "SWIMMER") == 0 || strcmp(cPlayerAnswer, "LIFEGUARD") == 0 || strcmp(cPlayerAnswer, "FIREFIGHTER") == 0 || strcmp(cPlayerAnswer, "PLUMBER") == 0)
		{
			return 1;
		}
	} else if (nQuestionIndex == 3) {
		if (strcmp(cPlayerAnswer, "CONCERT") == 0 || strcmp(cPlayerAnswer, "BAR") == 0 || strcmp(cPlayerAnswer, "CLUB") == 0 || strcmp(cPlayerAnswer, "MOVIE") == 0 || strcmp(cPlayerAnswer, "THEME_PARK") == 0 || strcmp(cPlayerAnswer, "RESTAURANT") == 0 || strcmp(cPlayerAnswer, "MALL") == 0) {
			return 1;
		}
	} else if (nQuestionIndex == 4) {
		if (strcmp(cPlayerAnswer, "HEIGHTS") == 0 || strcmp(cPlayerAnswer, "SPIDERS") == 0 || strcmp(cPlayerAnswer, "PUBLIC_SPEAKING") == 0 || strcmp(cPlayerAnswer, "FLYING") == 0 || strcmp(cPlayerAnswer, "OCEAN") == 0 || strcmp(cPlayerAnswer, "DARK") == 0) {
			return 1;
		}
	} else if (nQuestionIndex == 5) {
		if (strcmp(cPlayerAnswer, "PEPPERONI") == 0 || strcmp(cPlayerAnswer, "MUSHROOM") == 0 || strcmp(cPlayerAnswer, "SAUSAGE") == 0 || strcmp(cPlayerAnswer, "PINEAPPLE") == 0 || strcmp(cPlayerAnswer, "BACON") == 0 || strcmp(cPlayerAnswer, "HAM") == 0) {
			return 1;
		}
	} else if (nQuestionIndex == 6) {
		if (strcmp(cPlayerAnswer, "JANITOR") == 0 || strcmp(cPlayerAnswer, "JUDGE") == 0 || strcmp(cPlayerAnswer, "JEWELER") == 0 || strcmp(cPlayerAnswer, "JOCKEY") == 0 || strcmp(cPlayerAnswer, "JOURNALIST") == 0 || strcmp(cPlayerAnswer, "JUGGLER") == 0) {
			return 1;
		}
	} else if (nQuestionIndex == 7) {
		// FIX: replaced duplicated "HAMBURGER" with "BUTTER"
		if (strcmp(cPlayerAnswer, "MILK") == 0 || strcmp(cPlayerAnswer, "CHEESE") == 0 || strcmp(cPlayerAnswer, "STEAK") == 0 || strcmp(cPlayerAnswer, "HAMBURGER") == 0 || strcmp(cPlayerAnswer, "BUTTER") == 0) {
			return 1;
		}
	} else if (nQuestionIndex == 8) {
		if (strcmp(cPlayerAnswer, "ICE_CREAM") == 0 || strcmp(cPlayerAnswer, "LOLLIPOP") == 0 || strcmp(cPlayerAnswer, "STAMP") == 0 || strcmp(cPlayerAnswer, "POPSICLE") == 0 || strcmp(cPlayerAnswer, "ENVELOPE") == 0) {
			return 1;
		}
	} else if (nQuestionIndex == 9) {
		if (strcmp(cPlayerAnswer, "FIRE") == 0 || strcmp(cPlayerAnswer, "COFFEE") == 0 || strcmp(cPlayerAnswer, "TEA") == 0 || strcmp(cPlayerAnswer, "STOVE") == 0 || strcmp(cPlayerAnswer, "FIREPLACE") == 0)
		{
			return 1;
		}
	} else if (nQuestionIndex == 10) {
		if (strcmp(cPlayerAnswer, "ELEPHANT") == 0 || strcmp(cPlayerAnswer, "EAGLE") == 0 || strcmp(cPlayerAnswer, "EEL") == 0 || strcmp(cPlayerAnswer, "EMU") == 0) {
			return 1;
		}
	} else if (nQuestionIndex == 11) {
		if (strcmp(cPlayerAnswer, "THROW_UP") == 0 || strcmp(cPlayerAnswer, "SHOW_UP") == 0 || strcmp(cPlayerAnswer, "SLOW_UP") == 0 || strcmp(cPlayerAnswer, "BLOW_UP") == 0) {
			return 1;
		}
	} else if (nQuestionIndex == 12) {
		if (strcmp(cPlayerAnswer, "BED") == 0 || strcmp(cPlayerAnswer, "TRAMPOLINE") == 0 || strcmp(cPlayerAnswer, "COUCH") == 0 || strcmp(cPlayerAnswer, "PUDDLES") == 0) {
			return 1;
		}
	} else if (nQuestionIndex == 13) {
		if (strcmp(cPlayerAnswer, "ZOO") == 0 || strcmp(cPlayerAnswer, "ZIP") == 0 || strcmp(cPlayerAnswer, "ZAP") == 0 || strcmp(cPlayerAnswer, "ZEN") == 0) {
			return 1;
		}
	} else if (nQuestionIndex == 14) {
		if (strcmp(cPlayerAnswer, "INVEST") == 0 || strcmp(cPlayerAnswer, "SAVE_MONEY") == 0 || strcmp(cPlayerAnswer, "GAMBLE") == 0 || strcmp(cPlayerAnswer, "GET_A_JOB") == 0) {
			return 1;
		}
	}
	return 0;
}

/*
Description:	This function gives the lowest possible score (0 pts) to the user if they inputted an invalid answer
	@param - 	nPlayerAnswer - is the player's answer
				*pPlayerScore - is the player's score
				nPlayerTurn - is to give the turn to the player again

Precondition: 	nPlayerPoints, *pPlayer1Score, nPlayerTurn are non-negative integers

@return - 		nPlayerTurn - the player gets the turn again
*/
int giveJackpotWrongScore(int nPlayerAnswer, int *pPlayerScore, int nPlayerTurn) {
    int nCurrentRoundWinner = 0;
    int nPlayerPoints = 0; // FIX: initialize

    		if (nPlayerAnswer > -1) { // this expression checks if the user placed an answer
    		nPlayerPoints = 0; // FIX: simplified from rand() * 0; gives user 0 points for a wrong answer
       	 		*pPlayerScore += nPlayerPoints; // stores the points for the player
				nCurrentRoundWinner = 1;
		}
    	if (nCurrentRoundWinner == 1) { // this expression that the player gets to the turn again
        	return 1;
    	}
    return nPlayerTurn;
}

/*
Description:	This function determines the maximum score given for jackpot mode
	@param - 	nPlayerAnswer - is the player's answer
				*pPlayerScore - is the player's score
				nPlayerTurn - is to give the turn to the player again

Precondition: 	nPlayerPoints, *pPlayer1Score, nPlayerTurn are non-negative integers

@return - 		nPlayerTurn - the player gets the turn again
*/
int giveJackpotScore(int nPlayerAnswer, int *pPlayerScore, int nPlayerTurn) {
    int nCurrentRoundWinner = 0;
    int nPlayerPoints = 0; // FIX: initialize

    		if (nPlayerAnswer > -1) { // this expression checks if the user placed an answer
    		nPlayerPoints = awardPoints(); // generates points from 1-100
       	 		*pPlayerScore += nPlayerPoints; // stores the points for the player
				nCurrentRoundWinner = 1;
		}
    	if (nCurrentRoundWinner == 1) { // this expression that the player gets to the turn again
        	return 1;
    	}
    return nPlayerTurn;
}

/*
Description: 	This function displays the randomly generated questions, the choices, and the answer bar for jackpot mode

Precondition:	Only accepts string inputs therefore other characters, floating point characters, integer characters are not accepted as valid inputs
*/
void jackpotRound() {
	clearScreen();
	const char* cJackpotQuestions[15] = {
	"Name the most useful body part that begins with the letter \"L\"",
	"Name a musical instrument people strum.",
	"Name a profession that involves getting wet.",
	"Name a place that always has a long bathroom line.",
	"What is a common fear that many people have?",
	"What is a popular pizza topping?",
	"Name an occupation that begins with the letter \"J\"",
	"Name something in your refrigerator that you should thank a cow for",
	"Name a food that you might lick",
	"Name hot things",
	"Name an animal that starts with the letter \"E\"",
	"Name something you do that rhymes with \"grow up\"",
	"Name something kids just love to jump on.",
	"Give me a three-letter word that starts with the letter \"Z\"",
	"Name ways to get rich quickly"};
	int nTotalQuestions = 15;
	int nAsked[15] = {0}; // 0 means the question is unasked, so it sets all the questions unasked
	int nNumAsked;
	int nRandomQuestion;
	int pPlayerScore = 0;
    int nPlayerTurn = 1;
    int nIsAnswerCorrect = 0;

		for (nNumAsked = 0;nNumAsked < 5;) { // loops for 5 times
	 	nRandomQuestion = rand() % nTotalQuestions;

	 	if (nAsked[nRandomQuestion] == 0) { // checks if the question has been unasked
	 		char cPlayerAnswer[30]; // buffer to hold the generated string
	 		int nGivePlayerPoints = 1;
	 		size_t i;
	 		nAsked[nRandomQuestion] = 1; // if the question is unasked, this line stores the question as asked and ensures that it shouldn't repeat
	 		nNumAsked++; // increments if the question has been marked as asked
            clearScreen();
			printf ("Loading...\n");
			fflush(stdout);
			portableSleep (3);
            clearScreen(); // clears the loading screen
            printf ("%34s","- - - JACKPOT ROUND - - -\n\n");
            printf ("%29s","YOUR QUESTION IS\n\n");
            printf ("%s\n\n", cJackpotQuestions[nRandomQuestion]); // shows the randomly generated question (not repeated)

            	if (nPlayerTurn == 1) {
            		printf("Enter your answer in ALL CAPS and use (_) for spaces\n\n");
            		printf("Enter your answer: ");
            		fflush(stdout);
            		// FIX: bounded input (%29s) + return-value check to prevent buffer overflow
            		{
            			int nScanResult = scanf("%29s", cPlayerAnswer);
            			if (nScanResult == EOF) { // FIX: closed input ends the game instead of looping forever
            				g_inputClosed = 1;
            				printf ("\nInput closed. Ending the game...\n");
            				return;
            			}
            			clearInputBuffer();
            			if (nScanResult != 1) {
            				cPlayerAnswer[0] = '\0';
            			}
            		}
            		// FIX: normalize to uppercase so lowercase input still counts (case-insensitive)
            		for (i = 0; cPlayerAnswer[i] != '\0'; i++) {
            			cPlayerAnswer[i] = (char) toupper((unsigned char) cPlayerAnswer[i]);
            		}

            		// FIX: updated for new int return convention (1 = valid, 0 = invalid).
            		// A wrong answer scores 0 and moves on (no infinite re-prompt).
            		if (jackpotRoundAnswers(nRandomQuestion, cPlayerAnswer)) {
            		   nIsAnswerCorrect = 1;
            		} else {
            		   nIsAnswerCorrect = 0;
            		}
				}
			// FIX: score only when a new question was actually asked (was outside the if-block,
			// re-scoring the previous answer on repeated questions)
			if (nIsAnswerCorrect == 0) { // this expression validates if the player's answer is valid
			nPlayerTurn = giveJackpotWrongScore(nGivePlayerPoints, &pPlayerScore, nPlayerTurn);
			} else if (nIsAnswerCorrect == 1) {
			nPlayerTurn = giveJackpotScore(nGivePlayerPoints, &pPlayerScore, nPlayerTurn);
			}
    	}
	}
	divider();
	printf ("\nFINAL SCORE:\n"); // displays final scores
	printf("Your score is: %d points\n", pPlayerScore);

		if (pPlayerScore >= 200) { // determines if the player wins the jackpot round
			printf("Congratulations you win!\n\n");
			printf("Taking you to the main menu...");
			fflush(stdout);
		} else if (pPlayerScore < 200) {
			printf("Unfortunately you need %d more points to win the jackpot round\n\n", 200 - pPlayerScore);
			printf("Taking you to the main menu...");
			fflush(stdout);
	}
}

/*
Description:	Clears the terminal screen in a portable way.
				Uses "cls" on Windows and "clear" everywhere else.
				FIX: previous code used system("cls") unconditionally, which fails on Linux/macOS.
*/
void clearScreen() {
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
}

/*
Description:	Pauses the program until the user presses Enter.
				Uses system("pause") on Windows; on POSIX prints a prompt and waits for a newline.
				FIX: previous code used system("pause") unconditionally, which fails on Linux/macOS.
*/
void pauseScreen() {
#ifdef _WIN32
	system("pause");
#else
	printf("Press Enter to continue...");
	fflush(stdout);
	clearInputBuffer();
#endif
}

/*
Description:	Sleeps for the given number of seconds in a portable way.
				FIX: unistd.h sleep() does not exist on Windows; Sleep() is used there instead.
*/
void portableSleep(int nSeconds) {
	if (nSeconds < 0) {
		return;
	}
#ifdef _WIN32
	Sleep(nSeconds * 1000);
#else
	sleep((unsigned int) nSeconds);
#endif
}

/*
Description:	Discards remaining characters on the current input line.
				FIX: replaces fflush(stdin), which is undefined behavior in C.
				Sets g_inputClosed if end-of-file is reached.
*/
void clearInputBuffer() {
	int c;
	while ((c = getchar()) != '\n') {
		if (c == EOF) {
			g_inputClosed = 1;
			return;
		}
	}
}

/*
Description:	Reports whether stdin has reached end-of-file.
@return - 		1 if input is closed, 0 otherwise.
*/
int isInputClosed() {
	return g_inputClosed;
}

/*
Description:	Safely reads one integer from stdin.
	@param - 	pValue receives the parsed integer on success.
@return - 		1 on success, 0 if the input was not a valid integer,
				-1 if end-of-file was reached.
				Always consumes the rest of the input line, so invalid input
				cannot cause an infinite loop.
				FIX: previous code ignored scanf()'s return value, so typing
				letters left the variable uninitialized and looped forever.
*/
int readInt(int *pValue) {
	int nResult;
	if (pValue == NULL) {
		return 0;
	}
	nResult = scanf("%d", pValue);
	if (nResult == EOF) {
		g_inputClosed = 1;
		return -1;
	}
	clearInputBuffer(); // always consume the rest of the line
	if (nResult == 1) {
		return 1; // the value is valid even if EOF follows it without a newline
	}
	if (g_inputClosed) {
		return -1;
	}
	return 0;
}
