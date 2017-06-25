var gameStarted = false;

function ConnectToServer()
{
	connect();
    document.getElementById('NetworkConnect').style.visibility = 'hidden';
    document.getElementById('Leave').style.visibility = 'visible';
    drawMenu(2);
}

function Yes()
{
    sendConfirmation();
    document.getElementById('yes').style.visibility = 'hidden';
    document.getElementById('no').style.visibility = 'hidden';
    document.getElementById('question').style.visibility = 'hidden';
}

function No()
{
    Server.disconnect();
    document.getElementById('yes').style.visibility = 'hidden';
    document.getElementById('no').style.visibility = 'hidden';
    document.getElementById('question').style.visibility = 'hidden';
    document.getElementById('NetworkConnect').style.visibility = 'visible';
    document.getElementById('Leave').style.visibility = 'visible';
    document.getElementById('JoinCreateLobby').style.visibility = 'visible';
}

function JoinCreateLobby()
{
	sendCreateJoinLobby(document.getElementById('lobby').value);
    document.getElementById('JoinCreateLobby').style.visibility = 'hidden';
    document.getElementById('Spectate').style.visibility = 'hidden';
	document.getElementById('spectateText').style.visibility = 'hidden';
	document.getElementById('lobby').disabled = true;
}

function Lobby1()
{
	sendCreateJoinLobby("Lobby1");
	document.getElementById('Lobby1').style.visibility = 'hidden';
	document.getElementById('Lobby2').style.visibility = 'hidden';
	document.getElementById('Lobby3').style.visibility = 'hidden';
    document.getElementById('Lobby4').style.visibility = 'hidden';
    document.getElementById('Spectate1').style.visibility = 'hidden';
    document.getElementById('Spectate2').style.visibility = 'hidden';
    document.getElementById('Spectate3').style.visibility = 'hidden';
    document.getElementById('Spectate4').style.visibility = 'hidden';
}
function Lobby2()
{
	sendCreateJoinLobby("Lobby2");
	document.getElementById('Lobby1').style.visibility = 'hidden';
	document.getElementById('Lobby2').style.visibility = 'hidden';
	document.getElementById('Lobby3').style.visibility = 'hidden';
    document.getElementById('Lobby4').style.visibility = 'hidden';
    document.getElementById('Spectate1').style.visibility = 'hidden';
    document.getElementById('Spectate2').style.visibility = 'hidden';
    document.getElementById('Spectate3').style.visibility = 'hidden';
    document.getElementById('Spectate4').style.visibility = 'hidden';
}
function Lobby3()
{
	sendCreateJoinLobby("Lobby3");
	document.getElementById('Lobby1').style.visibility = 'hidden';
	document.getElementById('Lobby2').style.visibility = 'hidden';
	document.getElementById('Lobby3').style.visibility = 'hidden';
    document.getElementById('Lobby4').style.visibility = 'hidden';
    document.getElementById('Spectate1').style.visibility = 'hidden';
    document.getElementById('Spectate2').style.visibility = 'hidden';
    document.getElementById('Spectate3').style.visibility = 'hidden';
    document.getElementById('Spectate4').style.visibility = 'hidden';
}
function Lobby4()
{
	sendCreateJoinLobby("Lobby4");
	document.getElementById('Lobby1').style.visibility = 'hidden';
	document.getElementById('Lobby2').style.visibility = 'hidden';
	document.getElementById('Lobby3').style.visibility = 'hidden';
    document.getElementById('Lobby4').style.visibility = 'hidden';
    document.getElementById('Spectate1').style.visibility = 'hidden';
    document.getElementById('Spectate2').style.visibility = 'hidden';
    document.getElementById('Spectate3').style.visibility = 'hidden';
    document.getElementById('Spectate4').style.visibility = 'hidden';
}

function Spectate() {
    sendJoinSpectate();
    document.getElementById('Spectate').style.visibility = 'hidden';
    document.getElementById('JoinCreateLobby').style.visibility = 'hidden';
    document.getElementById('lobby').disabled = true;
	document.getElementById('spectateText').style.visibility = 'hidden';
}
function Start() 
{
	sendGameStartEvent();
	document.getElementById('Start').style.visibility = 'hidden';
	document.getElementById('Restart').style.visibility = 'hidden';
}

function Leave()
{
	ResetBoard();
	quitSession = true;
    Server.disconnect();
    document.getElementById('Start').style.visibility = 'hidden';
    document.getElementById('Leave').style.visibility = 'hidden';

	document.getElementById('question').style.visibility = 'hidden';
	document.getElementById('yes').style.visibility = 'hidden';
	document.getElementById('no').style.visibility = 'hidden';
    drawMenu(4);
	location.reload();
}

function Restart()
{
	sendGameStartEvent();
	document.getElementById('Start').style.visibility = 'hidden';
	document.getElementById('Restart').style.visibility = 'hidden';
}

function Refresh()
{
    sendStatisticsRetrievalEvent();
}

function RefreshLobbyList()
{
    sendRefreshLobbyListEvent();
}

function drawMenu(type) {
    // Get a reference to the canvas and indicate that we'll be working in 2D
    var canvas = document.getElementById("canvas");
    context = canvas.getContext("2d");

    // Get canvas dimensions
    WIDTH = canvas.width;
    HEIGHT = canvas.height;

    // Black borders (default)
    context.strokeStyle = "black";

    // For some reason, the canvas dimensions differ by one pixel when zooming in or out
    if (WIDTH != 600)
        WIDTH = 600;
    if (HEIGHT != 600)
        HEIGHT = 600;

    // Clear screen (erase everything)
    context.clearRect(0, 0, WIDTH, HEIGHT);

    if (type == 1) {
        var menu_width = 400;
        var menu_height = 200;
        context.fillStyle = "rgba(100, 100, 100, 1)";
        draw_block((WIDTH - menu_width) / 2, (HEIGHT - menu_height) / 2, menu_width, menu_height);

        context.fillStyle = "rgba(255, 255, 255, 1)";
        context.font = "25px Open Sans";
        context.fillText("CodeBombers Bomberman", (WIDTH - menu_width) / 2 + 25, (HEIGHT - menu_height) / 2 + 25, 500);

        context.font = "15px Open Sans";
        context.fillText("Welcome to the welcoming welcome screen!", (WIDTH - menu_width) / 2 + 30, (HEIGHT - menu_height) / 2 + 100, 500);
        context.fillText("To begin, sign into an account.", (WIDTH - menu_width) / 2 + 80, (HEIGHT - menu_height) / 2 + 160, 500);
    }
    else if (type == 2) {
        var menu_width = 400;
        var menu_height = 200;
        context.fillStyle = "rgba(100, 100, 100, 1)";
        draw_block((WIDTH - menu_width) / 2, (HEIGHT - menu_height) / 2, menu_width, menu_height);

        context.fillStyle = "rgba(255, 255, 255, 1)";
        context.font = "25px Open Sans";
        context.fillText("CodeBombers Bomberman", (WIDTH - menu_width) / 2 + 25, (HEIGHT - menu_height) / 2 + 25, 500);

        context.font = "15px Open Sans";
        context.fillText("Now we wait...", (WIDTH - menu_width) / 2 + 135, (HEIGHT - menu_height) / 2 + 100, 500);
        context.fillText("Nice day eh?", (WIDTH - menu_width) / 2 + 140, (HEIGHT - menu_height) / 2 + 160, 500);
    }
	else if (type == 3) {//DC Player Message
        var menu_width = 400;
        var menu_height = 200;
        context.fillStyle = "rgba(100, 100, 100, 1)";
        draw_block((WIDTH - menu_width) / 2, (HEIGHT - menu_height) / 2, menu_width, menu_height);

        context.fillStyle = "rgba(255, 255, 255, 1)";
        context.font = "25px Open Sans";
        context.fillText("CodeBombers Bomberman", (WIDTH - menu_width) / 2 + 25, (HEIGHT - menu_height) / 2 + 25, 500);

        context.font = "15px Open Sans";
        context.fillText("Player disconnected!", (WIDTH - menu_width) / 2 + 135, (HEIGHT - menu_height) / 2 + 100, 500);
        context.fillText("Returned back to lobby.", (WIDTH - menu_width) / 2 + 125, (HEIGHT - menu_height) / 2 + 160, 500);
    }
	else if (type == 4) {//DCMessage
        var menu_width = 400;
        var menu_height = 200;
        context.fillStyle = "rgba(100, 100, 100, 1)";
        draw_block((WIDTH - menu_width) / 2, (HEIGHT - menu_height) / 2, menu_width, menu_height);

        context.fillStyle = "rgba(255, 255, 255, 1)";
        context.font = "25px Open Sans";
        context.fillText("CodeBombers Bomberman", (WIDTH - menu_width) / 2 + 25, (HEIGHT - menu_height) / 2 + 25, 500);

        context.font = "15px Open Sans";
        context.fillText("You have disconnected!", (WIDTH - menu_width) / 2 + 130, (HEIGHT - menu_height) / 2 + 100, 500);
        context.fillText("Returned back to lobby.", (WIDTH - menu_width) / 2 + 125, (HEIGHT - menu_height) / 2 + 160, 500);
    }
	else if (type == 5) {//DCMessageLobby
        var menu_width = 400;
        var menu_height = 200;
        context.fillStyle = "rgba(100, 100, 100, 1)";
        draw_block((WIDTH - menu_width) / 2, (HEIGHT - menu_height) / 2, menu_width, menu_height);

        context.fillStyle = "rgba(255, 255, 255, 1)";
        context.font = "25px Open Sans";
        context.fillText("CodeBombers Bomberman", (WIDTH - menu_width) / 2 + 25, (HEIGHT - menu_height) / 2 + 25, 500);

        context.font = "15px Open Sans";
        context.fillText("Opponent disconnected! You win!", (WIDTH - menu_width) / 2 + 110, (HEIGHT - menu_height) / 2 + 100, 500);
        context.fillText("Returned back to lobby.", (WIDTH - menu_width) / 2 + 125, (HEIGHT - menu_height) / 2 + 160, 500);
    }
}
