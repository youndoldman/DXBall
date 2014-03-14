#include <string>
#include <memory>

#include "MenuManager.h"
#include "MenuList.h"
#include "Logger.h"

#include "math/RectHelpers.h"

MenuManager::MenuManager()
	:	currentGameState( GameState::MainMenu )
	,	prevGameState( GameState::Quit )

	,	hasGameStateChanged( false )
	,	isTwoPlayerMode( false )

	,	seletedGameID( -1 )
	,	lobbyStateChanged( false  )
{
	logger = Logger::Instance();
}
void MenuManager::CheckItemMouseOver( int x, int y )
{
	if ( currentGameState == GameState::Paused )
		CheckItemMouseOver_Pause( x, y );
	else if ( currentGameState == GameState::MainMenu )
		CheckItemMouseOver_MainMenu( x, y );
	else if ( currentGameState == GameState::Lobby )
		CheckItemMouseOver_Lobby( x, y );
}
void MenuManager::CheckItemMouseOver_MainMenu( int x, int y )
{
	for ( const auto &item : mainMenuItems )
		item.second->SetSelcted( false );

	MainMenuItemType mouseOver = CheckIntersections( x, y );

	if ( mouseOver != MainMenuItemType::Unknown )
		mainMenuItems[ mouseOver ]->SetSelcted( true );
}
void MenuManager::CheckItemMouseOver_Pause( int x, int y )
{
	for ( const auto &item : pauseMenuItems )
		item.second->SetSelcted( false );

	PauseMenuItemType mouseOver = CheckIntersections_Pause( x, y );

	if ( mouseOver != PauseMenuItemType::Unknown )
		pauseMenuItems[ mouseOver ]->SetSelcted( true );
}
void MenuManager::CheckItemMouseOver_Lobby( int x, int y )
{
	for ( const auto &item : lobbyMenuItems )
		item.second->SetSelcted( false );

	LobbyMenuItem mouseOver = CheckIntersections_Lobby( x, y );

	if ( mouseOver != LobbyMenuItem::Unknown && mouseOver != LobbyMenuItem::GameList )
		lobbyMenuItems[mouseOver]->SetSelcted( true );
}
bool MenuManager::CheckItemMouseClick( int x, int y)
{
	if ( currentGameState == GameState::Paused )
	{
		switch ( CheckIntersections_Pause( x, y))
		{
			case PauseMenuItemType::Resume:
				SetGameState( GameState::InGame );
				break;
			case PauseMenuItemType::MainMenu:
				SetGameState( GameState::MainMenu );
				break;
			case PauseMenuItemType::Quit:
				SetGameState( GameState::Quit );
				break;
			case PauseMenuItemType::Unknown:
				return false;
		}
	}
	else if ( currentGameState == GameState::MainMenu )
	{
		switch ( CheckIntersections( x, y))
		{
			case MainMenuItemType::SinglePlayer:
				SetGameState( GameState::InGame );
				isTwoPlayerMode = false;
				break;
			case MainMenuItemType::MultiPlayer:
				isTwoPlayerMode = true;
				SetGameState( GameState::Lobby );
				break;
			case MainMenuItemType::Options:
				break;
			case MainMenuItemType::Quit:
				SetGameState( GameState::Quit );
				break;
			case MainMenuItemType::Unknown:
				return false;
		}
	}
	else if ( currentGameState == GameState::Lobby )
	{
		lobbyState = CheckIntersections_Lobby( x, y);

		if ( lobbyState == LobbyMenuItem::GameList )
			seletedGameID = lobbyGameList->FindIntersectedItem( x, y );
		else
			seletedGameID = -1;

		lobbyStateChanged = true;
		return true;
	}

	return false;
}
MainMenuItemType MenuManager::CheckIntersections( int x, int y )
{
	if ( RectHelpers::CheckMouseIntersection( x, y, mainMenuItems[MainMenuItemType::SinglePlayer]->GetRect() ) )
		return MainMenuItemType::SinglePlayer;

	if ( RectHelpers::CheckMouseIntersection( x, y, mainMenuItems[MainMenuItemType::MultiPlayer]->GetRect() ) )
		return MainMenuItemType::MultiPlayer;

	if ( RectHelpers::CheckMouseIntersection( x, y, mainMenuItems[MainMenuItemType::Options]->GetRect() ) )
		return MainMenuItemType::Options;

	if ( RectHelpers::CheckMouseIntersection( x, y, mainMenuItems[MainMenuItemType::Quit]->GetRect() ) )
		return MainMenuItemType::Quit;

	return MainMenuItemType::Unknown;
}
PauseMenuItemType MenuManager::CheckIntersections_Pause( int x, int y )
{
	if ( RectHelpers::CheckMouseIntersection( x, y, pauseMenuItems[PauseMenuItemType::Resume]->GetRect() ) )
		return PauseMenuItemType::Resume;

	if ( RectHelpers::CheckMouseIntersection( x, y, pauseMenuItems[PauseMenuItemType::MainMenu]->GetRect() ) )
		return PauseMenuItemType::MainMenu;

	if ( RectHelpers::CheckMouseIntersection( x, y, pauseMenuItems[PauseMenuItemType::Quit]->GetRect() ) )
		return PauseMenuItemType::Quit;

	return PauseMenuItemType::Unknown;
}
LobbyMenuItem MenuManager::CheckIntersections_Lobby( int x, int y )
{
	if ( RectHelpers::CheckMouseIntersection( x, y, lobbyMenuItems[LobbyMenuItem::NewGame]->GetRect() ) )
		return LobbyMenuItem::NewGame;

	if ( RectHelpers::CheckMouseIntersection( x, y, lobbyMenuItems[LobbyMenuItem::Update]->GetRect() ) )
		return LobbyMenuItem::Update;

	if ( RectHelpers::CheckMouseIntersection( x, y, lobbyMenuItems[LobbyMenuItem::Back]->GetRect() ) )
		return LobbyMenuItem::Back;

	if ( RectHelpers::CheckMouseIntersection( x, y, lobbyGameList->GetRect() ) )
		return LobbyMenuItem::GameList;

	return LobbyMenuItem::Unknown;
}
void MenuManager::RemoevAllUnderscores( )
{
	//renderer.RemoveMainMenuItemsUnderlines();
}
GameState MenuManager::GetGameState() const
{
	return currentGameState;
}
GameState MenuManager::GetPrevGameState() const
{
	return prevGameState;
}
void MenuManager::SetGameState( const GameState &gs )
{
	if ( !IsGameStateChangeValid( gs ) )
		return;

	std::stringstream ss("");
	ss << "from " << static_cast< int32_t > ( currentGameState  )
	<< " to " << static_cast< int32_t > ( gs );

	logger->Log( __FILE__, __LINE__, "Changing game state : ", ss.str() );

	hasGameStateChanged = true;
	prevGameState = currentGameState;
	currentGameState = gs;
}
bool MenuManager::IsGameStateChangeValid( const GameState &gs) const
{
	if ( gs == currentGameState )
	{
		logger->Log( __FILE__, __LINE__, "GameState same as before!");
		return false;
	}

	return true;
}
bool MenuManager::HasGameStateChanged()
{
	if ( hasGameStateChanged )
	{
		hasGameStateChanged = false;
		return true;
	}

	return false;
}
GameState MenuManager::GoBackToPreviousMenuState()
{
	if ( currentGameState == GameState::MainMenu )
		SetGameState( GameState::Quit );
	else
		SetGameState( prevGameState );

	return currentGameState;
}
GameState MenuManager::GoToMenu()
{
	if ( currentGameState == GameState::MainMenu )
		SetGameState( GameState::Quit );
	else
		SetGameState( GameState::MainMenu );

	return currentGameState;
}
LobbyMenuItem MenuManager::GetLobbyState()
{
	return lobbyState;
}
bool MenuManager::HasLobbyStateChanged()
{
	bool state = lobbyStateChanged;
	lobbyStateChanged = false;
	return state;
}
bool MenuManager::WasGameStarted() const
{
	return
	(
		currentGameState == GameState::InGame &&
		prevGameState  != GameState::Lobby &&
		prevGameState  != GameState::Paused &&
		prevGameState  != GameState::InGameWait
	);
}
bool MenuManager::WasGameQuited() const
{
	return ( currentGameState != GameState::GameOver  && currentGameState != GameState::Paused && prevGameState == GameState::InGame );
}
bool MenuManager::WasGameResumed() const
{
	return ( currentGameState == GameState::InGame && prevGameState == GameState::Paused );
}
bool MenuManager::IsTwoPlayerMode() const
{
	return isTwoPlayerMode;
}
bool MenuManager::IsInAMenu() const
{
	return ( currentGameState == GameState::MainMenu
			|| currentGameState == GameState::Paused
			|| currentGameState == GameState::Lobby
		   );
}
void MenuManager::SetMainMenuItem( const MainMenuItemType &type, const std::shared_ptr< MenuItem >& button )
{
	mainMenuItems[type] = button;
}
void MenuManager::SetLobbyMenuItem( const LobbyMenuItem &type, const std::shared_ptr< MenuItem >  &button )
{
	lobbyMenuItems[type] = button;
}
void MenuManager::SetPauseMenuItem( const PauseMenuItemType &type, const std::shared_ptr< MenuItem >  &button )
{
	pauseMenuItems[type] = button;
}
