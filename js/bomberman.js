// Canvas
var context;
var WIDTH;
var HEIGHT;

// Board
var board;
var block_size = 40;

// Array of players
var player = [];

// Sprites
var white_bomberman;
var black_bomberman;
var red_bomberman;
var blue_bomberman;
var bomb_sprite;
var explosion_sprite;

// Utils
var playerText;
var paused = false;
var menu = true;
var gameOver = false;
var gameOverWinner;
var playerNumberInt;    //1-4   //Use this variable for int representation
var gameSessionNumberInt=-1;//Use -1 if not in a gamestate yet

// Network
var playernumber;			//IMPORTANT - playernumber is a string representation, not int.
var gameStarted = false;
var winnernumber;
var dead = false;

var p1x;
var p1y;
var p2x;
var p2y;
var p3x;
var p3y;
var p4x;
var p4y;

var interpolationModifier = 5; 

var quitSession = false;
function makeRandomID()
{
	// Credit/Source: http://stackoverflow.com/questions/1349404/generate-random-string-characters-in-javascript
    var text = "";
    var possible = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    for( var i=0; i < 8; i++ )
        text += possible.charAt(Math.floor(Math.random() * possible.length));

    document.getElementById('pid').value = text;
}

// Initialize everything
function init() {
	quitSession = false;
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

    // Disable smoothness for pixelated effect
    context.webkitImageSmoothingEnabled = false;

    // Load level map
    board = new Board(15, 15, 1);

    // Load sprites
	if (player1Connected)
	{
		white_bomberman = new Image();
		white_bomberman.src = 'assets/sprites/white_bomberman.gif';
	}else{}
    if (player2Connected)
	{
		black_bomberman = new Image();
		black_bomberman.src = 'assets/sprites/black_bomberman.gif';
	}else{}
    if (player3Connected)
	{
		red_bomberman = new Image();
		red_bomberman.src = 'assets/sprites/red_bomberman.gif';
	}else{}
    if (player4Connected)
	{
		blue_bomberman = new Image();
		blue_bomberman.src = 'assets/sprites/blue_bomberman.gif';
	}else{}
    bomb_sprite = new Image();
    explosion_sprite = new Image();
    powerups_sprite = new Image();
    bomb_sprite.src = 'assets/sprites/bombs.gif';
    explosion_sprite.src = 'assets/sprites/explosion.gif';
    powerups_sprite.src = 'assets/sprites/power-ups.gif';

    // Initialize players
	if (player1Connected)
	{
		player[0] = new Player(white_bomberman, "Chafic", 0, block_size, block_size);
	}
    if (player2Connected)
	{
		player[1] = new Player(black_bomberman, "Rachel", 1, WIDTH - 2 * block_size, block_size);
	}
    if (player3Connected)
	{
		player[2] = new Player(red_bomberman, "Richard", 2, block_size, HEIGHT - 2 * block_size);
	}
    if (player4Connected)
	{
		 player[3] = new Player(blue_bomberman, "Zouzou", 3, WIDTH - 2 * block_size, HEIGHT - 2 * block_size);
	}
	gameOverWinner ="";
	dead = false;
    paused = false;
    gameOver = false;
    menu = true;
	waiting = false;
	waitingTime = 0;
	frame=0;
	if (player1Connected)
	{
		p1x = player[0].x;
    	p1y = player[0].y;
	}
    if (player2Connected)
	{
		p2x = player[1].x;
    	p2y = player[1].y;
	}
    if (player3Connected) {
        p3x = player[2].x;
        p3y = player[2].y;
    }
    if(player4Connected)
    {
        p4x = player[3].x;
        p4y = player[3].y;
    }
	
    input();
}

// Game loop
function main() {
	if(quitSession == true)
	{
		return;
	}
	else
	{
		if(waiting)
		{
			var n = Date.now() - waitingTime;
			if(n >= 500)
			{
				waiting = false;
				waitingTime = 0;
				gameOver = true;
        		paused = false;
				gameOn = false;
			}
		}
		requestAnimationFrame(main);
		update();
		draw();
		interpolator();
	
		if (paused && !menu && !gameOver) {
			var pause_width = 140;
			var pause_height = 30;
			context.fillStyle = "rgba(100, 100, 100, 1)";
			draw_block((WIDTH-pause_width)/2, (HEIGHT-pause_height)/2, pause_width, pause_height);
	
			context.fillStyle = "rgba(255, 255, 255, 1)";
			context.font="25px Open Sans";
			context.fillText("PAUSE", (WIDTH-pause_width)/2+30, (HEIGHT-pause_height)/2+25, 500);
		} else if(menu && !gameOver) {
			var menu_width = 400;
			var menu_height = 200;
			context.fillStyle = "rgba(100, 100, 100, 1)";
			draw_block((WIDTH-menu_width)/2, (HEIGHT-menu_height)/2, menu_width, menu_height);
	
			context.fillStyle = "rgba(255, 255, 255, 1)";
			context.font="25px Open Sans";
			context.fillText("CodeBombers Bomberman", (WIDTH-menu_width)/2+25, (HEIGHT-menu_height)/2+25, 500);
	
			//This is used to display to the player what number and color they are.
			switch(playerNumberInt) {
				case 2:    // Player 2
					playerText = "You are Player 2 (Black)";
					break;
				case 3:    // Player 3
					playerText = "You are Player 3 (Red)";
					break;
				case 4:    // Player 4
					playerText = "You are Player 4 (Blue)";
					break;
				default:    // Player 1
					playerText = "You are Player 1 (White)";
					break;
			}
			context.font="20px Open Sans";
			context.fillText(playerText, (WIDTH-menu_width)/2+75, (HEIGHT-menu_height)/2+100, 500);
			context.fillText("Press the S key to Start Game", (WIDTH-menu_width)/2+50, (HEIGHT-menu_height)/2+160, 500);
		} else if (gameOver) {
			var menu_width = 400;
			var menu_height = 200;
			context.fillStyle = "rgba(100, 100, 100, 1)";
			draw_block((WIDTH-menu_width)/2, (HEIGHT-menu_height)/2, menu_width, menu_height);
	
			context.fillStyle = "rgba(255, 255, 255, 1)";
			context.font="25px Open Sans";
			context.fillText("Game Over!", (WIDTH-menu_width)/2+125, (HEIGHT-menu_height)/2+25, 500);
			context.font="20px Open Sans";
			context.fillText(gameOverWinner, (WIDTH-menu_width)/2+85, (HEIGHT-menu_height)/2+100, 500);
			context.fillText("Press the R key to Reset Game", (WIDTH-menu_width)/2+50, (HEIGHT-menu_height)/2+160, 500);
		}
	}
}

// Handle input
function input() {
    document.addEventListener('keydown', function(event) {
        if (menu) {
            switch (event.keyCode) {
                case 83:    // s, if menu then start game
                    sendMenuScreenStart();
                    break;
                default:
                    break;
            }
        } else if(gameOver) {
            switch (event.keyCode) {
                case 82:    // r
                    sendMenuScreenRestart();
                    break;
                default:
                    break;
            }
        }
        else if (!paused) {
            switch (event.keyCode) {
                case 37:    // left arrow
                    player[playerNumberInt-1].left = true;
                    break;
                case 38:    // up arrow
                    player[playerNumberInt-1].up = true;
                    break;
                case 39:    // right arrow
                    player[playerNumberInt-1].right = true;
                    break;
                case 40:    // down arrow
                    player[playerNumberInt-1].down = true;
                    break;
                case 32:    // space
                    if (player[playerNumberInt-1].pending_bombs.length < player[playerNumberInt-1].bomb_limit)
					{
						player[playerNumberInt-1].release_bomb = true;
					}
					else{log("pending bombs");}
                    break;
                default:
                    break;
            }
        }
    });

    document.addEventListener('keyup', function(event) {
        switch(event.keyCode) {
            case 37:    // left arrow
                player[playerNumberInt-1].left = false;
                player[playerNumberInt-1].frame["left"] = 0;
                break;
            case 38:    // up arrow
                player[playerNumberInt-1].up = false;
                player[playerNumberInt-1].frame["up"] = 0;
                break;
            case 39:    // right arrow
                player[playerNumberInt-1].right = false;
                player[playerNumberInt-1].frame["right"] = 12;
                break;
            case 40:    // down arrow
                player[playerNumberInt-1].down = false;
                player[playerNumberInt-1].frame["down"] = 0;
                break;
            default:
                break;
        }
    });

}

// Update game state
function update() {
		// Check if player is stepping on a power-up and have him pick it up
        var board_x = bitmap_position(player[playerNumberInt-1].x + player[playerNumberInt-1].sprite_width/2);
        var board_y = bitmap_position(player[playerNumberInt-1].y + player[playerNumberInt-1].sprite_height/2);
        var power_up = board.board_powerups[board_y][board_x];
        if (power_up != 0) 
		{
        	board.board_powerups[board_y][board_x] = 0;
            sendGetPowerUpsEvent(board_x, board_y, power_up);
        }
    	for (var i = 0; i < player.length; i++) {

        // Remove exploded bombs (which can happen even when the player is dead)
        if (typeof(player[i].bombs[0]) != "undefined" && player[i].bombs[0].extinguished)
        {
			player[i].bombs.shift();
			player[i].pending_bombs.shift();
		}

        // Update bomb state (which can happen even when the player is dead)
        for (var j = 0; j < player[i].bombs.length; j++)
            player[i].bombs[j].update();

        if (player[i].alive) {

            // Update player position and sprite animation
            player[i].move();


            // Release new bombs
            if (player[i].release_bomb) {

                // Bombs are placed on board tiles according to the position of the center of the player
                var x_bomb = pixel_position(bitmap_position(player[i].x + player[i].sprite_width/2));
                var y_bomb = pixel_position(bitmap_position(player[i].y + player[i].sprite_height/2));
				sendBombPlacementEvent(x_bomb,y_bomb);
                // Add bomb to the player's set of "released bombs"
				player[i].pending_bombs.push(1);
                player[i].release_bomb = false;
            }
        }
    }
}

function draw() {

    // Clear screen (erase everything)
    context.clearRect(0, 0, WIDTH, HEIGHT);

    // Fill background with pitch black
    context.fillStyle = "rgba(0, 0, 0, 1)";
    draw_block(0, 0, WIDTH, HEIGHT);

    // Draw power_ups
    for (var i = 0; i < board.height; i++)
        for (var j = 0; j < board.width; j++) {
            var powerup = board.board_powerups[i][j];
            if (powerup != 0 && board.level[i][j] == 0) {
                var sprite = fetch_sprite(board.powerups[powerup]);
                context.drawImage(powerups_sprite, sprite[0], sprite[1], sprite[2], sprite[3], pixel_position(j), pixel_position(i), sprite[2]*(block_size/sprite[3]), block_size);
            }
        }

    // Draw blocks
    for (var i = 0; i < 15; i++)
        for (var j = 0; j < 15; j++)
            if (board.level[j][i] == 1) {
                context.fillStyle = "rgba(255, 255, 255, 1)";
                draw_block(pixel_position(i), pixel_position(j), block_size, block_size);
            }
            else if (board.level[j][i] == 2) {
                context.fillStyle = "rgba(0, 255, 255, 1)";
                draw_block(pixel_position(i), pixel_position(j), block_size, block_size);
            }

    // Draw bombs
    for (var i = 0; i < player.length; i++)
        for (var j = 0; j < player[i].bombs.length; j++)
            player[i].bombs[j].draw();


    // Draw players
    for (var i = 0; i < player.length; i++)
        if (player[i].alive == true)
            player[i].draw();

}

function interpolator()
{
	//only interpolate players who aren't you
	if(playerNumberInt == 1)
	{
		if(player2Connected && player[1].x != p2x)
		{
			var x2 = p2x - player[1].x;
			var tweenX2 = Math.ceil(x2/interpolationModifier);
			player[1].x = player[1].x+tweenX2; 
		}
		if(player2Connected && player[1].y != p2y)
		{
			var y2 = p2y - player[1].y;
			var tweenY2 = Math.ceil(y2/interpolationModifier);
			player[1].y = player[1].y+tweenY2; 
		}
        if(player3Connected && player[2].x != p3x)
        {
            var x3 = p3x - player[2].x;
            var tweenX3 = Math.ceil(x3/interpolationModifier);
            player[2].x = player[2].x+tweenX3;
        }
        if(player3Connected && player[2].y != p3y)
        {
            var y3 = p3y - player[2].y;
            var tweenY3 = Math.ceil(y3/interpolationModifier);
            player[2].y = player[2].y+tweenY3;
        }
        if(player4Connected && player[3].x != p4x)
        {
            var x4 = p4x - player[3].x;
            var tweenX4 = Math.ceil(x4/interpolationModifier);
            player[3].x = player[3].x+tweenX4;
        }
        if(player4Connected && player[3].y != p4y)
        {
            var y4 = p4y - player[3].y;
            var tweenY4 = Math.ceil(y4/interpolationModifier);
            player[3].y = player[3].y+tweenY4;
        }
	}
	else if(playerNumberInt == 2)
	{
		if(player[0].x != p1x)
		{
			var x1 = p1x - player[0].x;
			var tweenX1 = Math.ceil(x1/interpolationModifier);
			player[0].x = player[0].x+tweenX1; 
		}
		if(player[0].y != p1y)
		{
			var y1 = p1y - player[0].y;
			var tweenY1 = Math.ceil(y1/interpolationModifier);
			player[0].y = player[0].y+tweenY1; 
		}
        if(player3Connected && player[2].x != p3x)
        {
            var x3 = p3x - player[2].x;
            var tweenX3 = Math.ceil(x3/interpolationModifier);
            player[2].x = player[2].x+tweenX3;
        }
        if(player3Connected && player[2].y != p3y)
        {
            var y3 = p3y - player[2].y;
            var tweenY3 = Math.ceil(y3/interpolationModifier);
            player[2].y = player[2].y+tweenY3;
        }
        if(player4Connected && player[3].x != p4x)
        {
            var x4 = p4x - player[3].x;
            var tweenX4 = Math.ceil(x4/interpolationModifier);
            player[3].x = player[3].x+tweenX4;
        }
        if(player4Connected && player[3].y != p4y)
        {
            var y4 = p4y - player[3].y;
            var tweenY4 = Math.ceil(y4/interpolationModifier);
            player[3].y = player[3].y+tweenY4;
        }
	} else if(playerNumberInt == 3)
	{
        if(player[0].x != p1x)
        {
            var x1 = p1x - player[0].x;
            var tweenX1 = Math.ceil(x1/interpolationModifier);
            player[0].x = player[0].x+tweenX1;
        }
        if(player[0].y != p1y)
        {
            var y1 = p1y - player[0].y;
            var tweenY1 = Math.ceil(y1/interpolationModifier);
            player[0].y = player[0].y+tweenY1;
        }
        if(player2Connected && player[1].x != p2x)
        {
            var x2 = p2x - player[1].x;
            var tweenX2 = Math.ceil(x2/interpolationModifier);
            player[1].x = player[1].x+tweenX2;
        }
        if(player2Connected && player[1].y != p2y)
        {
            var y2 = p2y - player[1].y;
            var tweenY2 = Math.ceil(y2/interpolationModifier);
            player[1].y = player[1].y+tweenY2;
        }
        if(player4Connected && player[3].x != p4x)
        {
            var x4 = p4x - player[3].x;
            var tweenX4 = Math.ceil(x4/interpolationModifier);
            player[3].x = player[3].x+tweenX4;
        }
        if(player4Connected && player[3].y != p4y)
        {
            var y4 = p4y - player[3].y;
            var tweenY4 = Math.ceil(y4/interpolationModifier);
            player[3].y = player[3].y+tweenY4;
        }
    } else if(playerNumberInt == 4)
    {
        if(player[0].x != p1x)
        {
            var x1 = p1x - player[0].x;
            var tweenX1 = Math.ceil(x1/interpolationModifier);
            player[0].x = player[0].x+tweenX1;
        }
        if(player[0].y != p1y)
        {
            var y1 = p1y - player[0].y;
            var tweenY1 = Math.ceil(y1/interpolationModifier);
            player[0].y = player[0].y+tweenY1;
        }
        if(player2Connected && player[1].x != p2x)
        {
            var x2 = p2x - player[1].x;
            var tweenX2 = Math.ceil(x2/interpolationModifier);
            player[1].x = player[1].x+tweenX2;
        }
        if(player2Connected && player[1].y != p2y)
        {
            var y2 = p2y - player[1].y;
            var tweenY2 = Math.ceil(y2/interpolationModifier);
            player[1].y = player[1].y+tweenY2;
        }
        if(player3Connected && player[2].x != p3x)
        {
            var x3 = p3x - player[2].x;
            var tweenX3 = Math.ceil(x3/interpolationModifier);
            player[2].x = player[2].x+tweenX3;
        }
        if(player3Connected && player[2].y != p3y)
        {
            var y3 = p3y - player[2].y;
            var tweenY3 = Math.ceil(y3/interpolationModifier);
            player[2].y = player[2].y+tweenY3;
        }
    }
}
