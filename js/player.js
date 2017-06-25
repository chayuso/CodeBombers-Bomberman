
function Player(sprite_sheet, name, number, x, y) {
    this.sprite_sheet = sprite_sheet;
    this.name = name;       // unused
    this.number = number;   // unused

    this.x = x+6;   // arbitrary addition to center the player inside the corresponding tile
    this.y = y;

    this.alive = true;
    this.invincible = false;    // unused (waiting for "vest" to be implemented)

    this.velocity = 3;

    this.sprite_width = null;
    this.sprite_height = null;

    // Indicated whether player is moving in a certain direction
    this.left = false;
    this.right = false;
    this.up = false;
    this.down = false;

    // Movement direction
    this.direction = "down";

    // Sprite animation
    this.frame = new Array(4);
    this.frame["left"] = 0;
    this.frame["up"] = 0;
    this.frame["right"] = 0;
    this.frame["down"] = 0;

    // Bomb information
    this.bombs = [];
	this.pending_bombs = [];
    this.release_bomb = false;
    this.bomb_radius = 1;
    this.bomb_limit = 1;

}

Player.prototype.move = function() {

    // Update position
    if (this.left)
        this.x -= this.velocity;
    else if (this.right)
        this.x += this.velocity;
    else if (this.up)
        this.y -= this.velocity;
    else if (this.down)
        this.y += this.velocity;

    // Collision detection (rectifies coordinates)
    var board_x_left = bitmap_position(this.x);
    var board_x_right = bitmap_position(this.x+this.sprite_width-1);
    var board_y_up = bitmap_position(this.y);
    var board_y_down = bitmap_position(this.y+this.sprite_height-1);

    var top_left_collision = board.level[board_y_up][board_x_left] >= 1;
    var top_right_collision = board.level[board_y_up][board_x_right] >= 1;
    var bottom_left_collision = board.level[board_y_down][board_x_left] >= 1;
    var bottom_right_collision = board.level[board_y_down][board_x_right] >= 1;

    if (this.left) {
        if (top_left_collision || bottom_left_collision) {
            this.x += this.velocity;
            board_x_left = bitmap_position(this.x);
        }
    }

    else if (this.right) {
        if (top_right_collision || bottom_right_collision) {
            this.x -= this.velocity;
            board_x_right = bitmap_position(this.x+this.sprite_width-1);
        }
    }

    else if (this.up) {
        if (top_left_collision || top_right_collision) {
            this.y += this.velocity;
            board_y_up = bitmap_position(this.y);
        }
    }

    else if (this.down) {
        if (bottom_left_collision || bottom_right_collision) {
            this.y -= this.velocity;
            board_y_down = bitmap_position(this.y+this.sprite_height-1);
        }
    }

    // Slide on corners
    if (this.left) {
        if (top_left_collision) {
            if (board.level[board_y_up+1][board_x_left] == 0 && board.level[board_y_up+1][board_x_left-1] == 0)
                this.y++;
        }

        else if (bottom_left_collision) {
            if (board.level[board_y_down-1][board_x_left] == 0 && board.level[board_y_down-1][board_x_left-1] == 0)
                this.y--;
        }
    }

    else if (this.right) {
        if (top_right_collision) {
            if (board.level[board_y_up+1][board_x_right] == 0 && board.level[board_y_up+1][board_x_right+1] == 0)
                this.y++;
        }

        else if (bottom_right_collision) {
            if (board.level[board_y_down-1][board_x_right] == 0 && board.level[board_y_down-1][board_x_right+1] == 0)
                this.y--;
        }
    }

    else if (this.up) {
        if (top_left_collision) {
            if (board.level[board_y_up][board_x_left+1] == 0 && board.level[board_y_up-1][board_x_left+1] == 0)
                this.x++;
        }

        else if (top_right_collision) {
            if (board.level[board_y_up][board_x_right-1] == 0 && board.level[board_y_up-1][board_x_right-1] == 0)
                this.x--;
        }
    }

    else if (this.down) {
        if (bottom_left_collision) {
            if (board.level[board_y_down][board_x_left+1] == 0 && board.level[board_y_down+1][board_x_left+1] == 0)
                this.x++;
        }

        else if (bottom_right_collision) {
            if (board.level[board_y_down][board_x_right-1] == 0 && board.level[board_y_down+1][board_x_right-1] == 0)
                this.x--;
        }
    }

    // Update animation frames
    if (this.left) {
        this.direction = "left";
        this.frame["left"] = (++this.frame["left"])%13;
    }
    else if (this.right) {
        this.direction = "right";
        this.frame["right"] = (++this.frame["right"])%13;
    }
    else if (this.up) {
        this.direction = "up";
        this.frame["up"] = (++this.frame["up"])%13;
    }
    else if (this.down) {
        this.direction = "down";
        this.frame["down"] = (++this.frame["down"])%13;
    }
}

Player.prototype.draw = function() {

    var sprite = [];

    switch (this.direction) {
        case "left":
            if (this.frame["left"] <= 4)
                sprite = fetch_sprite("horizontal_walk_1");
            else if (this.frame["left"] <= 8)
                sprite = fetch_sprite("horizontal_walk_2");
            else if (this.frame["left"] <= 12)
                sprite = fetch_sprite("horizontal_walk_3");
            break;

        case "up":
            if (this.frame["up"] <= 4)
                sprite = fetch_sprite("vertical_walk_4");
            else if (this.frame["up"] <= 8)
                sprite = fetch_sprite("vertical_walk_5");
            else if (this.frame["up"] <= 12)
                sprite = fetch_sprite("vertical_walk_6");
            break;

        case "right":
            if (this.frame["right"] <= 4)
                sprite = fetch_sprite("horizontal_walk_4");
            else if (this.frame["right"] <= 8)
                sprite = fetch_sprite("horizontal_walk_5");
            else if (this.frame["right"] <= 12)
                sprite = fetch_sprite("horizontal_walk_6");
            break;

        case "down":
            if (this.frame["down"] <= 4)
                sprite = fetch_sprite("vertical_walk_1");
            else if (this.frame["down"] <= 8)
                sprite = fetch_sprite("vertical_walk_2");
            else if (this.frame["down"] <= 12)
                sprite = fetch_sprite("vertical_walk_3");
            break;
        default:
            sprite = fetch_sprite("horizontal_walk_1");
            break;

			  }

    this.sprite_width = Math.floor(sprite[2]*(block_size/sprite[3]));
    this.sprite_height = block_size;
    context.drawImage(this.sprite_sheet, sprite[0], sprite[1], sprite[2], sprite[3], this.x, this.y, this.sprite_width, block_size);
}

// Kill the player if he is not invincible
Player.prototype.kill = function() {
    if (this.invincible == false)
	{
	//	if(this.number == playerNumberInt-1	)
	//	{
			this.alive = false;
	//	}
	}
}

Player.prototype.add_power_up = function(power_up) {
    switch(power_up) {
        //case 1:
            // It gives the player the ability to pick up, carry, and then throw bombs (not yet implemented)
            //console.log("power_glove");
            //break;
        //case 2:
            // Allows player to punch bombs in order to knock them away (not yet implemented)
            //console.log("boxing_glove");
            //break;
        case 1:
            // Increases fire range by 1
            console.log("fire");
            this.bomb_radius++;
            break;
        case 2:
            // Increases player's speed by 1
            console.log("skate");
            this.velocity++;
            break;
        //case 5:
            // Releases a line of bomb
            //console.log("line_bomb");
            //break;
        //case 6:
            // Player is immune to all dangers for a fixed period of time
            //this.invincible = true;
            //break;
        //case 7:
            // Gets one of the following "bad" effects for a fixed period of time:
            // Dizzy: controls are switched, left becomes right and up becomes down
            // Constipation: player unable to set bombs
            // Diarrhea: player continuously sets bombs when possible
            //console.log("skull");
            //break;
        case 3:
            // Decreases the player's speed by 1
            console.log("geta");
            if (this.velocity > 1)
                this.velocity--;
            break;
        //case 9:
            // Gives the ability to "kick" bombs by walking into them, which sends the bomb sliding across the stage until it collides with a wall, player, or another bomb
            //console.log("kick");
            //break;
        case 4:
            // Increases the player's number of bombs by 1
            console.log("bomb");
            this.bomb_limit++;
            break;
    }
}
Player.prototype.reduce_power_up = function (power_up) {
    switch (power_up) {
        //case 1:
        // It gives the player the ability to pick up, carry, and then throw bombs (not yet implemented)
        //console.log("power_glove");
        //break;
        //case 2:
        // Allows player to punch bombs in order to knock them away (not yet implemented)
        //console.log("boxing_glove");
        //break;
        case 1:
            // Decreases fire range by 1
            console.log("fire");
            if (this.bomb_radius > 1)
                this.bomb_radius--;
            break;
        case 2:
            // Decreases player's speed by 1
            console.log("skate");
            if (this.velocity > 1)
                this.velocity--;
            break;
            //case 5:
            // Releases a line of bomb
            //console.log("line_bomb");
            //break;
            //case 6:
            // Player is immune to all dangers for a fixed period of time
            //this.invincible = true;
            //break;
            //case 7:
            // Gets one of the following "bad" effects for a fixed period of time:
            // Dizzy: controls are switched, left becomes right and up becomes down
            // Constipation: player unable to set bombs
            // Diarrhea: player continuously sets bombs when possible
            //console.log("skull");
            //break;
        case 3:
            // Increases the player's speed by 1
            console.log("geta");
            this.velocity++;
            break;
            //case 9:
            // Gives the ability to "kick" bombs by walking into them, which sends the bomb sliding across the stage until it collides with a wall, player, or another bomb
            //console.log("kick");
            //break;
        case 4:
            // Decreases the player's number of bombs by 1
            console.log("bomb");
            if (this.bomb_limit > 1)
                this.bomb_limit--;
            break;
    }
}
