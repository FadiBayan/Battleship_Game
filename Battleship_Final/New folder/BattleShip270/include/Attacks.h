#ifndef ATTACKS
#define ATTACKS

/**
 * Simulates firing at a specified coordinate on the opponent's grid.
 * 
 * Mechanism:
 * - Parse the input coordinates (e.g., "A5") into grid indices.
 * - Check if the coordinate is valid and whether the cell has already been attacked.
 * - If valid:
 *   - Update the grid with a `Hit` or `Miss` marker.
 *   - Return a message ("Hit!" or "Miss") through `outputMsg`.
 * - If the cell has already been attacked or the coordinates are invalid, return an error code.
 *
 * @param coords     String representing user input coordinates (e.g., "A5").
 * @param attacked   2D character array representing the opponent's grid.
 * @param easyMode   Game difficulty level; affects how misses are handled.
 * @param outputMsg  Pointer to a string to store the result of the shot ("Hit!" or "Miss").
 * @return           1 if the shot is valid, -1 if the coordinates are invalid or the cell was already hit.
 */
int Fire(char *coords, char** attacked, int easyMode, char ** outputMsg);

/**
 * Performs a radar sweep in a 2x2 region starting at the specified coordinate to detect enemy ships.
 * 
 * Mechanism:
 * - Parse the input coordinates into grid indices.
 * - Validate the coordinate to ensure it can form a 2x2 region within the grid.
 * - Iterate over the 2x2 region and check for the presence of any ship cells and also skip if the cell is 
 * true on the smoke grid.
 * - Return the result through `outputMsg` ("Enemy ships found" or "No enemy ships found").
 *
 * @param inputC    String representing the starting coordinate of the radar sweep.
 * @param player    Pointer to the player performing the radar sweep.
 * @param outputMsg Pointer to a string to store the result ("Enemy ships found" or "No enemy ships found").
 * @return          1 for a valid radar sweep, -1 if coordinates are invalid.
 */
int performRadarSweep(char *inputC, Player* player, char ** outputMsg);

/**
 * Applies a 2x2 smoke screen in the specified region to obscure cells from being targeted.
 * 
 * Mechanism:
 * - Parse the input coordinates into grid indices.
 * - Validate that the coordinate can form a 2x2 region within the grid.
 * - Update the corresponding cells on the smokegrid . 
 * - Return a confirmation message through `outputMsg` ("Smoke screen applied.") or an error if conditions are not met.
 *
 * @param coords     String representing the starting coordinate for the smoke screen.
 * @param player     Pointer to the player applying the smoke screen.
 * @param opponent   Pointer to the opponent.
 * @param outputMsg  Pointer to a string to store the result ("Smoke screen applied." or an error message).
 * @return           1 if the smoke screen is applied successfully, -1 if coordinates are invalid or if conditions are not met.
 */
int applySmokeScreen(char *coords, Player* player, Player * opponent, char ** outputMsg);

/**
 * Uses artillery to attack a 2x2 region starting at the specified coordinate. Can only be used immediately after sinking a ship.
 * 
 * Mechanism:
 * - Parse the input coordinates into grid indices.
 * - Validate that the coordinate can form a 2x2 region within the grid.
 * - Check if the artillery usage condition is met (a ship has been sunk recently).
 * - Iterate over the 2x2 region and update the grid with `Hit` or `Miss` markers.
 * - Return the result of the attack through `outputMsg` ("Hit!" or "Miss!") or an error message if conditions are not met.
 *
 * @param inputC     String representing the starting coordinate for the artillery attack.
 * @param player     Pointer to the player using the artillery.
 * @param opp        Pointer to the opponent being attacked.
 * @param difficulty Game difficulty level; affects how misses are handled.
 * @param outputMsg  Pointer to a string to store the result of the attack ("Hit!" or "Miss!").
 * @return           1 if the artillery is successfully used, -1 if conditions are not met or coordinates are invalid.
 */
int Artillery(char *inputC, Player* player, Player* opp, int difficulty, char ** outputMsg);

/**
 * Fires a torpedo along a row or column starting at the specified coordinate. Can only be used after sinking the opponent's third ship.
 * 
 * Mechanism:
 * - Parse the input coordinates into grid indices.
 * - Validate the coordinate and determine the direction (row or column).
 * - Check if the torpedo usage condition is met (after sinking the opponent's third ship).
 * - Traverse the row or column starting from the specified coordinate and update cells with `Hit` or `Miss`.
 * - Return the result of the attack through `outputMsg` ("Hit!" or "Miss!") or an error message if conditions are not met.
 *
 * @param inputC     String representing the starting coordinate for the torpedo attack.
 * @param player     Pointer to the player using the torpedo.
 * @param opp        Pointer to the opponent being attacked.
 * @param difficulty Game difficulty level; affects how misses are handled.
 * @param outputMsg  Pointer to a string to store the result of the attack ("Hit!" or "Miss!").
 * @return           1 if the torpedo is successfully used, -1 if conditions are not met or coordinates are invalid.
 */
int Torpedo(char *inputC,Player* player,Player* opp, int difficulty, char ** outputMsg);

#endif
