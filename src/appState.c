#include "../includes/appState.h"
#include "../includes/gameState.h"
#include "../includes/leaderboard.h"
#include "../includes/save.h"

/*------------------------------------------------------------------------------
 * Retorna o appState default
 *----------------------------------------------------------------------------*/
type_appState getDefaultAppState(){
  type_appState defaultAppState = {0};

  //Variaveis de controle do app
  defaultAppState.appStatus = STATUS_RUNNING;

  //Variaveis de controle do jogo
  defaultAppState.gameState = getDefaultGameState();

  //Variaveis de controle da tela
  defaultAppState.screen.lastScreen = SCREEN_NONE;
  defaultAppState.screen.currentScreen = SCREEN_MENU;
  defaultAppState.screen.menuState = STATE_MENU_FIRST + 1;

  return defaultAppState;
}

/*------------------------------------------------------------------------------
 * Retorna um novo appState com base na ação realizada na tela de menu
 *----------------------------------------------------------------------------*/
void handleMenuAction(type_appState * appState){
  switch (appState->userAction){
    case ACTION_UP:
      appState->screen.menuState--;
      if(appState->screen.menuState == STATE_MENU_FIRST)
        appState->screen.menuState = STATE_MENU_LAST - 1;
      break;

    case ACTION_DOWN:
      appState->screen.menuState++;
      if(appState->screen.menuState == STATE_MENU_LAST)
        appState->screen.menuState = STATE_MENU_FIRST + 1;
      break;

    case ACTION_ENTER:
      switch (appState->screen.menuState){
        case STATE_MENU_CONTINUE:
          readSaveFile(appState, "teste");

          appState->screen.currentScreen = SCREEN_GAME;
          break;

        case STATE_MENU_NEWGAME:
          newGame(appState);
          break;

        case STATE_MENU_HELP:
          appState->screen.currentScreen = SCREEN_HELP;
          break;

        case STATE_MENU_EXIT:
          appState->appStatus = STATUS_OK;
          break;

        default:break;
      }
      break;

    case ACTION_ESCAPE:
      appState->appStatus = STATUS_OK;
      break;

    default:
      break;
  }
}

/*------------------------------------------------------------------------------
 * Retorna um novo appState com base na ação realizada no appState atual
 *----------------------------------------------------------------------------*/
void handleUserAction(type_appState * appState){
  if(appState->userAction != ACTION_NONE){
    switch (appState->screen.currentScreen){
      case SCREEN_MENU:
        appState->screen.lastScreen = SCREEN_MENU;
        handleMenuAction(appState);
        break;

      case SCREEN_GAME:
        appState->screen.lastScreen = SCREEN_GAME;
        switch (appState->userAction){
          case ACTION_UP:
          case ACTION_DOWN:
          case ACTION_LEFT:
          case ACTION_RIGTH:
            handleGameAction(appState);
            break;

          case ACTION_GAME_NEW:
            newGame(appState);
            break;

          case ACTION_GAME_UNDO:
            undoGameAction(appState);
            break;
            
          case ACTION_GAME_PRE_SAVE:
            appState->screen.lastPopup = appState->screen.currentPopup;
            appState->screen.currentPopup = POPUP_SAVE;
            break;

          case ACTION_GAME_SAVE:
            writeSaveFile(&appState->gameState, &appState->userString);
            appState->screen.lastPopup = appState->screen.currentPopup;
            appState->screen.currentPopup = POPUP_NONE;
            break;

          case ACTION_PRE_ESCAPE:
            appState->screen.lastPopup = appState->screen.currentPopup;
            appState->screen.currentPopup = POPUP_ESCAPE;
            break;

          case ACTION_ESCAPE:
            appState->screen.lastPopup = POPUP_NONE;
            appState->screen.currentPopup = POPUP_NONE;
            appState->screen.currentScreen = SCREEN_MENU;
            break;

          default:
            appState->screen.lastPopup = appState->screen.currentPopup;
            appState->screen.currentPopup = POPUP_NONE;
            break;
        }
        break;

      case SCREEN_ENDGAME:
        appState->screen.lastScreen = SCREEN_ENDGAME;
        switch (appState->userAction){
           case ACTION_ENTER:
            //Adiciona o novo resultado a struct leaderboard na ultima posição, que não aparece na tela
            strcpy(appState->leaderboard.name[11], appState->userString);
            appState->leaderboard.points[11] = appState->gameState.score;

            //Organiza a struct em ordem decrescente
            bubbleSortLeaderboard(&appState->leaderboard);

            //Salva a struct em arquivo
            writeLeaderboardFile(&appState->leaderboard);

            appState->screen.currentScreen = SCREEN_MENU;
            break;

          case ACTION_ESCAPE:
            appState->screen.currentScreen = SCREEN_MENU;
            break;

          default:break;
        }
        break;

      case SCREEN_HELP:
        appState->screen.lastScreen = SCREEN_HELP;
        switch (appState->userAction){
          case ACTION_ESCAPE:
            appState->screen.currentScreen = SCREEN_MENU;
            break;

          default:break;
        }
        break;

      default:
        appState->screen.currentScreen = SCREEN_MENU;
        break;
    }
  }
}
