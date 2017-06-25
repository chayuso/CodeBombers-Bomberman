var Server;
var online = false;

var PLAYER_1 = 1;
var PLAYER_2 = 2;
var PLAYER_3 = 3;
var PLAYER_4 = 4;

var player1Connected = false;
var player2Connected = false;
var player3Connected = false;
var player4Connected = false;

var changedPlayer; // initialize early to save computational cycles for critical gameplay
var direction; // initialize early to save computational cycles for critical gameplay

var latencyLoopInterval = 500; // in milliseconds
var latencyMessages = []; // 2^32 possible elements
var latencyMessageID = 0;
var latencyLoopID = -1;

var initialTimestamp;
var EstimatedRTT=0;

var gameOn = false;
var waiting = false;
var waitingTime;

var frame = 0;

// Custom Server Commands to send to server:
//{"event": "addVirtualClientEvent", "client": 5}
//{"event": "removeVirtualClientEvent", "client": 5}

//{"event": "destroyAllBlocksCustomEvent"}

/* Begin TA Functions */
function log( text ) 
{
	$log = $('#log');
	//Add text to log
	$log.append(($log.val()?"\n":'')+text);
	//Autoscroll
	$log[0].scrollTop = $log[0].scrollHeight - $log[0].clientHeight;
}

function chatlog( text )
{
    $chatlog = $('#chatlog');
    //Add text to log
    $chatlog.append(($chatlog.val()?"\n":'')+text);
    //Autoscroll
    $chatlog[0].scrollTop = $chatlog[0].scrollHeight - $chatlog[0].clientHeight;
}

function statsTable( text )
{
    $statstable = $('#statstable');
    //Add text to log
    $statstable.append(($statstable.val()?"\n":'')+text);
    //Autoscroll
    $statstable[0].scrollTop = $statstable[0].scrollHeight - $statstable[0].clientHeight;
}

function lobbyList( text )
{
    $lobbyList = $('#lobbyList');
    //Add text to log
    $lobbyList.append(($lobbyList.val()?"\n":'')+text);
    //Autoscroll
    $lobbyList[0].scrollTop = $lobbyList[0].scrollHeight - $lobbyList[0].clientHeight;
}

function connect()
{
	log('[Client] Connecting...');
	Server = new FancyWebSocket('ws://' + document.getElementById('ip').value + ':' + document.getElementById('port').value);

	// Input handler
	$('#message').keypress(function(e) {
		if ( e.keyCode == 13 && this.value ) {
			log( '[Client] ' + this.value );
			send( this.value );

			
			$(this).val('');
		}
	});

    // Chat Handler
    $('#chat').keypress(function(e) {
        if ( e.keyCode == 13 && this.value ) {
            chatlog( 'Me: ' + this.value );
            sendChatMessage(document.getElementById('pid').value, this.value );

            $(this).val('');
        }
    });

    // Lobby create/join Handler
    $('#lobby').keypress(function (e) {
        if (e.keyCode == 13 && this.value) {
            log('[Client] Create or join ' + this. value);
            JoinCreateLobby();
			//sendCreateJoinLobby(this.value);
        }
    });
	// Connected handler
	Server.bind('open', function() 
	{
		log("[Client] Connected.");
		sendSetPlayerIDEvent(1, document.getElementById('pid').value,document.getElementById('pw').value);
		log("[Client] ID sent.");
		initialTimestamp = Date.now();
		latencyLoopID = setInterval(doLatencyEstimation, latencyLoopInterval);
		log("[Client] Registered latency estimation loop (" + latencyLoopInterval + " ms).");
	});

	// Disconnect handler
	Server.bind('close', function( data ) 
	{
		if(!quitSession)
		{
			ResetBoard();
			quitSession = true;
			document.getElementById('Start').style.visibility = 'hidden';
    		document.getElementById('Leave').style.visibility = 'hidden';

    		drawMenu(4);
		}	
		player1Connected = false;
        player2Connected = false;
        player3Connected = false;
        player4Connected = false;
		gameOver = false;
		gameOn = false;
		gameOverWinner ="";

		document.getElementById('refreshButton').style.visibility = 'hidden';
        document.getElementById('lobby').disabled = true;

		document.getElementById('JoinCreateLobby').style.visibility = 'hidden';
		document.getElementById('Spectate').style.visibility = 'hidden';
		document.getElementById('spectateText').style.visibility = 'hidden';

		document.getElementById('Start').style.visibility = 'hidden';
    	document.getElementById('Leave').style.visibility = 'hidden';
		document.getElementById('question').style.visibility = 'hidden';
		document.getElementById('yes').style.visibility = 'hidden';
		document.getElementById('no').style.visibility = 'hidden';
		log( "[Client] Disconnected, or no server found." );
		clearInterval(latencyLoopID);
		latencyMessages = [];
		latencyMessageID = 0;
		document.getElementById('NetworkConnect').style.visibility = 'visible';
	});

	// Log any messages sent from server
	Server.bind('message', function( payload ) 
	{
		try
		{
			var theJSON = JSON.parse(payload);
			var firedEvent = theJSON.event;

            if (firedEvent == "latencyEstimationEvent")
			{
				handleLatencyEstimation(theJSON.id, theJSON.X, theJSON.Y);
				if(gameOn)
				{	
					if(frame < Number(theJSON.id))
					{
						//frame = Number(theJSON.id);
						if(playerNumberInt == 1) {
                            player[1].x = p2x;
                            player[1].y = p2y;
                            p2x = Number(theJSON.player2X);
                            p2y = Number(theJSON.player2Y);
                            
                            if (player3Connected){
                                player[2].x = p3x;
                                player[2].y = p3y;
                                p3x = Number(theJSON.player3X);
                           		p3y = Number(theJSON.player3Y);
                        	}
                            if(player4Connected) {
                                player[3].x = p4x;
                                player[3].y = p4y;
                                p4x = Number(theJSON.player4X);
                                p4y = Number(theJSON.player4Y);
                            }
						}
						else if(playerNumberInt == 2)
						{
							player[0].x = p1x;
							player[0].y = p1y;
							p1x = Number(theJSON.player1X);
							p1y = Number(theJSON.player1Y);
                            
                            if(player3Connected) {
                                player[2].x = p3x;
                                player[2].y = p3y;
                                p3x = Number(theJSON.player3X);
                                p3y = Number(theJSON.player3Y);
                            }
                            if(player4Connected) {
                                player[3].x = p4x;
                                player[3].y = p4y;
                                p4x = Number(theJSON.player4X);
                                p4y = Number(theJSON.player4Y);
                            }
						}
                        else if(playerNumberInt == 3)
                        {
                            
                            player[0].x = p1x;
                            player[0].y = p1y;
                            p1x = Number(theJSON.player1X);
                            p1y = Number(theJSON.player1Y);
                            player[1].x = p2x;
                            player[1].y = p2y;
                            p2x = Number(theJSON.player2X);
                            p2y = Number(theJSON.player2Y);
                            if(player4Connected) {
                                player[3].x = p4x;
                                player[3].y = p4y;
                                p4x = Number(theJSON.player4X);
                                p4y = Number(theJSON.player4Y);
                            }
                        }
                        else if(playerNumberInt == 4)
                        {
                          
                            player[0].x = p1x;
                            player[0].y = p1y;
                            p1x = Number(theJSON.player1X);
                            p1y = Number(theJSON.player1Y);
                            player[1].x = p2x;
                            player[1].y = p2y;
                            p2x = Number(theJSON.player2X);
                            p2y = Number(theJSON.player2Y);
                            if(player3Connected) {
                                player[2].x = p3x;
                                player[2].y = p3y;
                                p3x = Number(theJSON.player3X);
                                p3y = Number(theJSON.player3Y);
                            }
                        }
					}
				}
			}
			else if(firedEvent == "bombPlacementEvent")
			{
				log("[Server] Player"+theJSON.player+" Bomb Placed!");
				var t = EstimatedRTT/2;//(Date.now()-Number(theJSON.clientSentTime)-Number(theJSON.serverBombElapsedTime))/2;
				player[Number(theJSON.player)-1].bombs.push(new Bomb(bomb_sprite,
                            explosion_sprite,
                            Number(theJSON.xBomb),
                            Number(theJSON.yBomb),
                            player[Number(theJSON.player)-1].bomb_radius,
							Number(t)		  					  ));
				log("Client Recieved Bomb. Player: "+theJSON.player);
			}	
			else if (firedEvent == "statisticsEvent")
            {
            	statsTable("Name: " + theJSON.player + " Wins: " + theJSON.wins + " Lossess: " + theJSON.losses + " Ties: " + theJSON.ties + " Deaths: " + theJSON.deaths + " Kills: " + theJSON.kills + " Bombs Placed: " + theJSON.bombs);
	        }
	        else if (firedEvent == "refreshLobbyListEvent")
            {
                lobbyList(theJSON.name + "\t" + theJSON.players + "/4\t" + theJSON.gameStatus);
            }
            else if (firedEvent == "gameStartedEvent")
			{
					log("[Server] Game started!");
					gameStarted = true;
					gameOver = false;
                	init();
                	ResetBoard();
					main();
  
			}
			else if (firedEvent == "wrongPasswordEvent")
			{
				Server.disconnect();
			    log("[Server] Wrong Password. Please reenter password.");
			    document.getElementById('NetworkConnect').style.visibility = 'visible';
			}
			else if (firedEvent == "createNewAccountEvent")
			{
			    document.getElementById('question').style.visibility = 'visible';
			    document.getElementById('yes').style.visibility = 'visible';
			    document.getElementById('no').style.visibility = 'visible';
			}
			else if(firedEvent == "menuScreenStartedEvent")
			{
				log("[Server] Loading Powerups");
				var n = 0;
				var sArray = theJSON.boardPowerUps.split(",")
				for (var i = 0; i < 15; i++)
				{
					for (var j = 0; j < 15; j++)
					{
						board.board_powerups[i][j] = Number(sArray[n]);
						n++;
					}
				}
				frame = 0;
				menu = false;
				gameOn = true;
                paused = false;
				log("[Server] Menu screen started!");
			}
			else if (firedEvent == "destroyAllBlocksCustomEvent")
			{
				log("[Server] Destroying All Blocks");
				for (var i = 0; i < 15; i++)
				{
					for (var j = 0; j < 15; j++)
					{
						if (board.level[i][j] == 1) 
						{
            				board.level[i][j] = 0;
						}
					}
				}	
			}
			else if(firedEvent == "menuScreenRestartedEvent")
			{	
                init();
				ResetBoard();
				log("[Server] Loading Powerups");
				var n = 0;
				var sArray = theJSON.boardPowerUps.split(",")
				for (var i = 0; i < 15; i++)
				{
					for (var j = 0; j < 15; j++)
					{
						board.board_powerups[i][j] = Number(sArray[n]);
						n++;
					}
				}
                frame = 0;
                menu = false;
                gameOver = false;
                gameOn = true;
				gameOverWinner="";
                paused = false;
				log("[Server] Menu screen restarted");

			}
			else if(firedEvent == "playerDeathEvent")
			{
				log("[Server] Player Death");
				if(Number(theJSON.player)!=playerNumberInt)
				{player[Number(theJSON.player)-1].kill();}
				if(Number(theJSON.gameOver) == 1)
				{
					log("[Server] GameOver");
					if(Number(theJSON.winner) == 0)
					{
						log("[Server] Game Finished!");
						log("[Server] Readjusting Connection Speed: 500ms");
						latencyLoopInterval = 500;
						clearInterval(latencyLoopID);
						LatencyLoopID = setInterval(doLatencyEstimation, latencyLoopInterval);
						gameOverWinner = "            Tie Game!!!";
						winnernumber = 0;
						waiting = false;
						waitingTime = 0;
						paused = false;
						gameOn = false;
						gameOver = true;
					}
					else if(gameOverWinner!="            Tie Game!!!")
					{
						latencyLoopInterval = 500;
						clearInterval(latencyLoopID);
						LatencyLoopID = setInterval(doLatencyEstimation, latencyLoopInterval);
						if(Number(theJSON.winner) == 1)
						{
							gameOverWinner = "Player 1 (White) Wins!!!";
							winnernumber = 1;
						}else if(Number(theJSON.winner) == 2)
						{
							gameOverWinner = "Player 2 (Black) Wins!!!";
							winnernumber = 2;
						}
						else if(Number(theJSON.winner) == 3)
						{
							gameOverWinner = "Player 3 (Red) Wins!!!";
							winnernumber = 3;
						}
						else if(Number(theJSON.winner) == 4)
						{
							gameOverWinner = "Player 4 (Blue) Wins!!!";
							winnernumber = 4;
						}
						paused = true;
						waiting = true;
						waitingTime = Date.now();
						log("[Server] Game Finished!");
						log("[Server] Readjusting Connection Speed: 500ms");
					}else{
						log("check gameOverWinner");
						log(theJSON.winner);
						log(gameOverWinner);}
				}
			}	
			else if (firedEvent == "updateCommunicationThrottleEvent")
			{
				log("[Server] Readjusted Communication Speed: "+theJSON.communicationSpeed+"ms");
				latencyLoopInterval = Number(theJSON.communicationSpeed);
				clearInterval(latencyLoopID);
				latencyLoopID = setInterval(doLatencyEstimation, latencyLoopInterval);
			}
			else if (firedEvent == "updatePlayerNumberEvent")
			{
				playernumber = theJSON.player;
				playerNumberInt = Number(playernumber);
				log("[Server] You are Player #" + playernumber + ".");
				log("[Server] Your ID is " + document.getElementById('pid').value);
				if (playernumber == PLAYER_1)
				{
                    player1Connected = true;
					log("[Client] Waiting for Players to join...");
				}
				else if (playernumber == PLAYER_2)
				{
                    player2Connected = true;
					log("[Client] Retrieving Players' information...");
				}else if (playernumber == PLAYER_3)
                {
                    player3Connected = true;
                    log("[Client] Retrieving Players' information...");
                }else if (playernumber == PLAYER_4)
                {
                    player4Connected = true;
                    log("[Client] Retrieving Players' information...");
                }
			}
			else if (firedEvent == "playerConnectedEvent")
			{
				var connectedPlayer = theJSON.player;
				if (connectedPlayer == PLAYER_1) // Player 1 has connected, only player 2 gets this event
				{
                    player1Connected = true;
					player2Connected = true;
					log("[Client] Player #1 has joined the networked game.");
					log("[Client] Player #1's ID is " + theJSON.id);
					log("[Client] Waiting for Player #1 to start the game...");
                }
				else if (connectedPlayer == PLAYER_2) // Player 2 has connected, only player 1 gets this event
				{
					player1Connected = true;
                    player2Connected = true;
					log("[Client] Player #2 has joined the networked game.");
					log("[Client] Player #2's ID is " + theJSON.id);
					log("[Client] Ready to start the game...");
					if(playerNumberInt == 1)
					{
						document.getElementById('Start').style.visibility = 'visible';
					}
				}
				else if (connectedPlayer == PLAYER_3) // Player 3 has connected, only player 1,2 gets this event
                {
                    player1Connected = true;
                    player2Connected = true;
                	player3Connected = true;
                    log("[Client] Player #3 has joined the networked game.");
                    log("[Client] Player #3's ID is " + theJSON.id);
                    log("[Client] Ready to start the game...");
                }
                else if (connectedPlayer == PLAYER_4) // Player 4 has connected, only player 1,2,3 gets this event
                {
                	player1Connected = true;
                    player2Connected = true;
                    player3Connected = true;
                	player4Connected = true
                    log("[Client] Player #4 has joined the networked game.");
                    log("[Client] Player #4's ID is " + theJSON.id);
                    log("[Client] Ready to start the game...");
                }
			}
			else if (firedEvent == "clientNetworkConnectedEvent")
			{

			    log("[Client] Welcome " + theJSON.id + "! Create or Join a lobby.");
			    document.getElementById('lobby').disabled = false;

				document.getElementById('refreshButton').style.visibility = 'visible';
				document.getElementById('JoinCreateLobby').style.visibility = 'visible';
				//document.getElementById('Spectate').style.visibility = 'visible';
				//document.getElementById('spectateText').style.visibility = 'visible';

			}
			else if (firedEvent == "networkConnectedEvent")
			{
				log("[Server] "+ theJSON.id + " connected to the network!");	
			}
			else if (firedEvent == "rejectLobbyFullEvent")
			{

			    log("[Server] Failed to join Lobby! Lobby Full!");
			    document.getElementById('lobby').disabled = false;	
				document.getElementById('JoinCreateLobby').style.visibility = 'visible';
				document.getElementById('refreshButton').style.visibility = 'visible';
				//document.getElementById('Spectate').style.visibility = 'visible';
				//document.getElementById('spectateText').style.visibility = 'visible';
			}
			else if (firedEvent == "rejectLobbyJoinEvent")
			{
			    log("[Server] Failed to join lobby! Lobby In-Game!");
			    document.getElementById('lobby').disabled = false;
				document.getElementById('JoinCreateLobby').style.visibility = 'visible';
				document.getElementById('refreshButton').style.visibility = 'visible';
				//document.getElementById('Spectate').style.visibility = 'visible';
				//document.getElementById('spectateText').style.visibility = 'visible';
			}
			else if (firedEvent == "rejectLobbyCreateEvent")
			{
			    log("[Server] Failed to join lobby! Lobby Space Full!");
			    document.getElementById('lobby').disabled = false;
				document.getElementById('JoinCreateLobby').style.visibility = 'visible';
				document.getElementById('refreshButton').style.visibility = 'visible';	
				//document.getElementById('Spectate').style.visibility = 'visible';
				//document.getElementById('spectateText').style.visibility = 'visible';


			}
			else if (firedEvent == "rejectLobbyJoinEvent")
			{
				log("[Server] Failed to join lobby! Lobby In-Game!");	
				document.getElementById('lobby').disabled = false;
                document.getElementById('JoinCreateLobby').style.visibility = 'visible';
				document.getElementById('refreshButton').style.visibility = 'visible';
                //document.getElementById('Spectate').style.visibility = 'visible';
				//document.getElementById('spectateText').style.visibility = 'visible';
			}
			else if (firedEvent == "rejectLobbyCreateEvent")
			{
				log("[Server] Failed to join lobby! Lobby Space Full!");
				document.getElementById('lobby').disabled = false;
                document.getElementById('JoinCreateLobby').style.visibility = 'visible';
                //document.getElementById('Spectate').style.visibility = 'visible';
				//document.getElementById('spectateText').style.visibility = 'visible';
				document.getElementById('refreshButton').style.visibility = 'visible';

            }
            else if (firedEvent == "acceptSpecJoinEvent")
            {
                log("[Client] " + theJSON.id + " is now spectating...")
            }
            else if (firedEvent == "rejectSpecJoinEvent")
            {
                log("[Server] Failed to find Lobby " + theJSON.lobby + ".")
                document.getElementById('lobby').disabled = false;
                document.getElementById('JoinCreateLobby').style.visibility = 'visible';
                //document.getElementById('Spectate').style.visibility = 'visible';
				//document.getElementById('spectateText').style.visibility = 'visible';
				document.getElementById('refreshButton').style.visibility = 'visible';

            }
			else if (firedEvent == "rejectSpectatorFullEvent")
            {
                log("[Server] Failed to join. Spectator Lobby is full.")
                document.getElementById('lobby').disabled = false;
                document.getElementById('JoinCreateLobby').style.visibility = 'visible';
                //document.getElementById('Spectate').style.visibility = 'visible';
                //document.getElementById('spectateText').style.visibility = 'visible';
                document.getElementById('refreshButton').style.visibility = 'visible';
            }
			else if (firedEvent == "playerConnectedEvent")
			{
				var connectedPlayer = theJSON.player;
				if (connectedPlayer == PLAYER_1) // Player 1 has connected, only player 2 gets this event
				{
                    player1Connected = true;
					player2Connected = true;
					log("[Client] Player #1 has joined the networked game.");
					log("[Client] Player #1's ID is " + theJSON.id);
					log("[Client] Waiting for Player #1 to start the game...");
                }
				else if (connectedPlayer == PLAYER_2) // Player 2 has connected, only player 1 gets this event
				{
					player1Connected = true;
                    player2Connected = true;
					log("[Client] Player #2 has joined the networked game.");
					log("[Client] Player #2's ID is " + theJSON.id);
					log("[Client] Ready to start the game...");
					if(playernumber == PLAYER_1)
						document.getElementById('Start').style.visibility = 'visible';
				}
				else if (connectedPlayer == PLAYER_3) // Player 3 has connected, only player 1,2 gets this event
                {
                    player1Connected = true;
                    player2Connected = true;
                	player3Connected = true;
                    log("[Client] Player #3 has joined the networked game.");
                    log("[Client] Player #3's ID is " + theJSON.id);
                    log("[Client] Ready to start the game...");
                }
                else if (connectedPlayer == PLAYER_4) // Player 4 has connected, only player 1,2,3 gets this event
                {
                	player1Connected = true;
                    player2Connected = true;
                    player3Connected = true;
                	player4Connected = true
                    log("[Client] Player #4 has joined the networked game.");
                    log("[Client] Player #4's ID is " + theJSON.id);
                    log("[Client] Ready to start the game...");
                }
			}	
			else if (firedEvent == "powerUpsEvent")
			{
			    board.board_powerups[Number(theJSON.yPowerUp)][Number(theJSON.xPowerUp)] = 0;
			    if (theJSON.addOrReduce == "+")
			    {
			        player[Number(theJSON.player)-1].add_power_up(theJSON.powerUp);
			        log("[Server] Player " + (Number(theJSON.player)) + " Get powerUp");
			    }
			    else if (theJSON.addOrReduce == "-")
			    {
			        player[Number(theJSON.player)-1].reduce_power_up(theJSON.powerUp);
			        log("[Server] Player " + (Number(theJSON.player)) + " didn't get powerUp");
			    }  
			}
			else if(firedEvent == "playerLobbyLeaveEvent")
			{
				ResetBoard();
				quitSession = true;
				menu = false;

    			document.getElementById('Start').style.visibility = 'hidden';

    			drawMenu(5);
				

    			log("[Client] Welcome " + theJSON.id + "! Create or Join a lobby.");
    			document.getElementById('lobby').disabled = false;
				document.getElementById('JoinCreateLobby').style.visibility = 'visible';
				document.getElementById('refreshButton').style.visibility = 'visible';
				//document.getElementById('Spectate').style.visibility = 'visible';
				//document.getElementById('spectateText').style.visibility = 'visible';

				
			}
			else if(firedEvent == "playerGameLeaveEvent")
			{
				log("[Client] Player "+theJSON.playerNumber+" Disconnected");
				if(Number(theJSON.playerNumber) == 1)
				{
					player1Connected = false;
				}
				else if(Number(theJSON.playerNumber) == 2)
				{
					player2Connected = false;
				}
				else if(Number(theJSON.playerNumber) == 3)
				{
					player3Connected = false;
				}
				else if(Number(theJSON.playerNumber) == 4)
				{
					player4Connected = false;
				}
			}
			else if (firedEvent == "playerDisconnectEvent")
			{
				gameOverWinner ="";
				ResetBoard();
				quitSession = true;
				gameOver = false;
    		
    			document.getElementById('Start').style.visibility = 'hidden';

    			drawMenu(5);
				

    			log("[Client] Welcome " + theJSON.id + "! Create or Join a lobby.");
    			document.getElementById('lobby').disabled = false;
				document.getElementById('JoinCreateLobby').style.visibility = 'visible';
				document.getElementById('refreshButton').style.visibility = 'visible';
				//document.getElementById('Spectate').style.visibility = 'visible';
				//document.getElementById('spectateText').style.visibility = 'visible';


				player1Connected = false;
                player2Connected = false;
                player3Connected = false;
                player4Connected = false;
				if(gameOn)
				{
					log("[Server] Readjusting Connection Speed: 500ms");
					latencyLoopInterval = 500;
					clearInterval(latencyLoopID);
					LatencyLoopID = setInterval(doLatencyEstimation, latencyLoopInterval);
					gameOn = false;
				}
				var disconnectedPlayer = theJSON.player;
				if (disconnectedPlayer == PLAYER_1) // Player 1 has disconnected, so this client is Player 2, 3, or 4
				{
					player1Connected = false;
					if (gameStarted == true || playernumber == PLAYER_2 || playernumber == PLAYER_3 || playernumber == PLAYER_4) // player #2,3,4 will never have "gameStarted" to true since it doesnt press button
					{
						clearInterval(game_interval_ID);
						dc_message(PLAYER_1);
						log("[Client] Player 1 has disconnected, so the game ended.");
						gameStarted = false;
					}
					else
					{
						log("[Client] Player 1 has disconnected.");
					}
					log("[Client] Player #1 was the leader, so the server has closed the game room and disconnected you.");
				}
				else if (disconnectedPlayer == PLAYER_2) // Player 2 has disconnected, so this client is Player 1,3, or 4
				{
                    player2Connected = false;
					if (gameStarted == true && player3Connected == false && player4Connected == false)
					{
                        gameStarted = false;
						clearInterval(game_interval_ID);
						dc_message(PLAYER_2);
						log("[Client] Player 2 has disconnected, so the game ended.");
					}
					else
					{
						log("[Client] Player 2 has disconnected.");
					}
				}
                else if (disconnectedPlayer == PLAYER_3) // Player 3 has disconnected, so this client is Player 1,2, or 4
                {
                    player3Connected = false;
                    if (gameStarted == true && player2Connected == false && player4Connected == false)
                    {
                        gameStarted = false;
                        clearInterval(game_interval_ID);
                        dc_message(PLAYER_3);
                        log("[Client] Player 3 has disconnected, so the game ended.");
                    }
                    else
                    {
                        log("[Client] Player 3 has disconnected.");
                    }
                }
                else if (disconnectedPlayer == PLAYER_4) // Player 4 has disconnected, so this client is Player 1,2, or 3
                {
                    player4Connected = false;
                    if (gameStarted == true && player2Connected == false && player3Connected == false)
                    {
                        gameStarted = false;
                        clearInterval(game_interval_ID);
                        dc_message(PLAYER_4);
                        log("[Client] Player 4 has disconnected, so the game ended.");
                    }
                    else
                    {
                        log("[Client] Player 4 has disconnected.");
                    }
                }
                if(player2Connected == false && player3Connected == false && player4Connected == false) {
                    document.getElementById('Restart').style.visibility = 'hidden';
                    document.getElementById('Start').style.visibility = 'show';
                    log("[Client] Waiting for Players to join...");
                }
			} 
			else if(firedEvent == "playerChatEvent")
			{
				chatlog(theJSON.player + ": " + theJSON.msg);
			}
		}
		catch (err)
		{
			log("[Server] " + payload);
			log(err); // <- If a JSON error occurs, or no event matched, then it prints out the message. Use to your advantage!
			//console.log(err); //<- Since this is a try, catch, if you see your event being printed in raw JSON, that means there's an error. Try looking at the err.
		}
	});
	
	Server.connect();
}

function send( text ) 
{
	Server.send( 'message', text );
}

function doLatencyEstimation()
{
	sendLatencyEstimationEvent(latencyMessageID++, Date.now() - initialTimestamp);
}

function handleLatencyEstimation(id, X, Y)
{
	var B = Date.now() - initialTimestamp;
    var SampleRTT = B - latencyMessages[id] - (Y-X);
    EstimatedRTT = Math.round(0.875*EstimatedRTT + 0.125*SampleRTT);
    document.getElementById('RTT').value = EstimatedRTT + "ms";
	//log("[Debug] Latency ID: " + id + " | A: " + latencyMessages[id] + " | X: " + X + " | Y: " + Y + " | B: " + B);
	//log("[Client] Latency Estimation (ID #" + id + "): " + EstimatedRTT + " ms.");
	// A: Timestamp of Client when he sent the packet.
	// B: Timestamp of Client when he received the reply.
	// X: Timestamp of Server when he received the packet.
	// Y: Timestamp of Server when he sent the reply.
	
	// Reason for this terminology: http://stackoverflow.com/questions/1228089/how-does-the-network-time-protocol-work
}

/* Begin Custom Functions */
function sendSetPlayerIDEvent(player, id, pw)
{
	send("{\"event\": \"setPlayerIDEvent\", \"player\": " + player + ", \"id\":\"" + id + "\", \"pw\":\"" + pw + "\"}"); // JSON example: JSON example -> {"event": "setPlayerIDEvent", "player": 1, "id": "TTaiN"}
}
function sendGameStartEvent()
{
	send("{\"event\": \"gameStartEvent\"" + "}"); // JSON example: JSON example -> {"event": "gameStartEvent"}
}
function sendGameFinishedEvent(winner)
{
	send("{\"event\": \"gameFinishedEvent\", \"winner\": " + winner + "}"); // JSON example: JSON example -> {"event": "gameFinishedEvent"}
}
function sendBombPlacementEvent(xBomb, yBomb)
{
	send("{\"event\": \"bombPlacementEvent\", \"sentTime\": \"" + Date.now() + "\", \"xBomb\": " + xBomb + ", \"yBomb\": " + yBomb + ", \"player_number\": " + playerNumberInt + "}");
	log("Client Sent Bomb. Player: "+playernumber);
}
function sendGetPowerUpsEvent(xPowerUp, yPowerUp, powerUp) 
{
    send("{\"event\": \"powerUpsEvent\", \"sentTime\": \"" + Date.now() + "\", \"xPowerUp\": " + xPowerUp + ", \"yPowerUp\": " + yPowerUp + ", \"powerUp\": " + powerUp + ", \"player_number\": " + playerNumberInt +"}");
}
function sendPlayerDeath(player)
{
	send("{\"event\": \"playerDeathEvent\", \"player\": " + player + "}");
}
function sendLatencyEstimationEvent(id, timestamp)
{
	if(gameOn)
	{
		send("{\"event\": \"latencyEstimationEvent\", \"id\": " + id + ", \"xPosition\": " + player[playerNumberInt-1].x + ", \"yPosition\": " + player[playerNumberInt-1].y + "}");
	}
	else
	{
		send("{\"event\": \"latencyEstimationEvent\", \"id\": " + id + ", \"xPosition\": 1000, \"yPosition\": 1000}");
	}
	latencyMessages.push(timestamp);
}
function sendMenuScreenStart()
{
	send("{\"event\": \"menuScreenStart\"" + "}"); // JSON example: JSON example -> {"event": "gameStartEvent"}
}
function sendMenuScreenRestart()
{
	send("{\"event\": \"menuScreenRestart\"" + "}"); // JSON example: JSON example -> {"event": "gameStartEvent"}
}
function sendChatMessage(id, msg)
{
	send("{\"event\": \"playerChatEvent\", \"playerID\": \"" +id+ "\"" + ", \"msg\": \"" +msg+ "\"}");
}
function sendConfirmation()
{
    send("{\"event\": \"confirmationEvent\", \"player\": " + 1 + ", \"id\":\"" + document.getElementById('pid').value + "\", \"pw\":\"" + document.getElementById('pw').value + "\"}"); // JSON example: JSON example -> {"event": "newPlayerIDEvent", "player": 1, "id": "TTaiN"}
}
function sendRejection() {
    send("{\"event\": \"rejectionEvent\"" + "}");
}
function sendCreateJoinLobby(lobby_name)
{
	send("{\"event\": \"createJoinEvent\", \"lobbyName\": \"" +lobby_name+ "\"" +", \"id\":\"" + document.getElementById('pid').value + "\"}");  
}

function sendJoinSpectate(lobby_name) {
    send("{\"event\": \"joinSpectateEvent\", \"lobbyName\": \"" + document.getElementById('lobby').value + "\"" + ", \"id\":\"" + document.getElementById('pid').value + "\"}");
}

function sendStatisticsRetrievalEvent() {
    $statstable = $('#statstable');
    //Clear text in log
    $statstable.html("");
    send("{\"event\": \"statisticsEvent\"" + "}");
}
function sendRefreshLobbyListEvent() {
    $lobbyList = $('#lobbyList');
    //Clear text in list
    $lobbyList.html("");
    send("{\"event\": \"refreshLobbyListEvent\"" + "}");
}