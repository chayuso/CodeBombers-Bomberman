
function Board(width, height, level_number) {

    // Board dimensions
    this.width = width;
    this.height = height;

    // Load white and blue blocks of the specified level
    this.level = level[level_number];

    this.powerups = {
         1: "fire",
         2: "skate",
         3: "geta",
         4: "bomb"
    }


    // Create 2D array that will store generated power-ups (same dimensions as board of blocks)
    this.board_powerups = create_2D_array(height, width);

    // Randomly distribute power-ups across board
    this.load_power_ups();

}

// Clean powerup board first
Board.prototype.load_power_ups = function() {
    for (var i = 0; i < this.height; i++)
        for (var j = 0; j < this.width; j++)
                this.board_powerups[i][j] = 0;
}
