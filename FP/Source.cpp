#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <ctime>
#include <sstream>
#include <string>

using namespace std;

const int MAIN_MENU = 0;
const int DIFFICULTY_MENU = 1;
const int IN_GAME = 2;
const int GAME_OVER = 3;

const int EASY = 0;
const int MEDIUM = 1;
const int HARD = 2;

const float PADDING = 10.0f;
const int MAX_CARDS = 40; // 20 countries + 20 capitals

const string countries[] = {
    "USA", "Canada", "Mexico", "France", "Germany", "Italy", "Spain", "Brazil", "Argentina", "China",
    "Japan", "India", "Australia", "Russia", "South Africa", "Egypt", "Turkey", "Saudi Arabia", "Iran", "Iraq"
};

const string capitals[] = {
    "Washington D.C.", "Ottawa", "Mexico City", "Paris", "Berlin", "Rome", "Madrid", "Brasilia", "Buenos Aires", "Beijing",
    "Tokyo", "New Delhi", "Canberra", "Moscow", "Pretoria", "Cairo", "Ankara", "Riyadh", "Tehran", "Baghdad"
};

int getGridSize(int difficulty) {
    switch (difficulty) {
    case EASY: return 3;   // 3x3 grid
    case MEDIUM: return 4; // 4x4 grid
    case HARD: return 5;   // 5x5 grid
    default: return 3;     // Default to easy
    }
}

int getTimeLimit(int difficulty) {
    switch (difficulty) {
    case EASY: return 60;  // 1 minute for 3x3
    case MEDIUM: return 90; // 1 minute 30 seconds for 4x4
    case HARD: return 120; // 2 minutes for 5x5
    default: return 60;    // Default to 1 minute
    }
}

sf::Vector2f getCardSizeAndPosition(int gridSize, const sf::RenderWindow& window) {
    float cardSize = (min(window.getSize().x, window.getSize().y) - (PADDING * (gridSize + 1))) / gridSize;
    return { cardSize, cardSize };
}

void drawSquare(sf::RenderWindow& window, float x, float y, float size, const sf::Color& fillColor, const sf::Color& outlineColor) {
    sf::RectangleShape square(sf::Vector2f(size, size));
    square.setPosition(x, y);
    square.setFillColor(fillColor);
    square.setOutlineColor(outlineColor);
    square.setOutlineThickness(2);
    window.draw(square);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(700, 700), "Find The Pair - Countries and Capitals");

    sf::Font font;
    if (!font.loadFromFile("C:/Users/Waina Malik/source/repos/FP/FP/assets/fonts/BERNHC.ttf")) {
        cerr << "Error loading font\n";
        return -1;
    }

    // Define text objects for the game
    sf::Text text, attemptText, scoreText, timerText, gameOverText, noMoreTimeText, noMoreAttemptsText;
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);

    attemptText.setFont(font);
    attemptText.setCharacterSize(20);
    attemptText.setFillColor(sf::Color::White);
    attemptText.setPosition(10, 10); // Position for attempts

    scoreText.setFont(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(600, 10); // Position for score on the right side

    timerText.setFont(font);
    timerText.setCharacterSize(20);
    timerText.setFillColor(sf::Color::White);
    timerText.setPosition((window.getSize().x - timerText.getGlobalBounds().width) / 2, 30); // Centered position for timer

    gameOverText.setFont(font);
    gameOverText.setString("Game Over! Press Enter to Restart or Escape to Exit");
    gameOverText.setCharacterSize(24);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setPosition(50, 300); // Centered position for game over message

    noMoreTimeText.setFont(font);
    noMoreTimeText.setString("No more time left!");
    noMoreTimeText.setCharacterSize(24);
    noMoreTimeText.setFillColor(sf::Color::Red);
    noMoreTimeText.setPosition(50, 250); // Position for no more time message

    noMoreAttemptsText.setFont(font);
    noMoreAttemptsText.setString("No more attempts left!");
    noMoreAttemptsText.setCharacterSize(24);
    noMoreAttemptsText.setFillColor(sf::Color::Red);
    noMoreAttemptsText.setPosition(50, 250); // Position for no more attempts message

    // Define text objects for the main menu
    sf::Text startText, difficultyText, easyText, mediumText, hardText, exitText;
    startText.setFont(font);
    startText.setString("Start Game");
    startText.setCharacterSize(30);
    startText.setFillColor(sf::Color::White);
    startText.setPosition(300, 50); // Centered in 800x800

    difficultyText.setFont(font);
    difficultyText.setString("Select Difficulty");
    difficultyText.setCharacterSize(30);
    difficultyText.setFillColor(sf::Color::White);
    difficultyText.setPosition(300, 150); // Centered in 800x800

    easyText.setFont(font);
    easyText.setString("Easy");
    easyText.setCharacterSize(30);
    easyText.setFillColor(sf::Color::White);
    easyText.setPosition(300, 200); // Position for Easy option

    mediumText.setFont(font);
    mediumText.setString("Medium");
    mediumText.setCharacterSize(30);
    mediumText.setFillColor(sf::Color::White);
    mediumText.setPosition(300, 250); // Position for Medium option

    hardText.setFont(font);
    hardText.setString("Hard");
    hardText.setCharacterSize(30);
    hardText.setFillColor(sf::Color::White);
    hardText.setPosition(300, 300); // Position for Hard option

    exitText.setFont(font);
    exitText.setString("Exit");
    exitText.setCharacterSize(30);
    exitText.setFillColor(sf::Color::White);
    exitText.setPosition(300, 350); // Position for Exit option

    int selectedOption = 0;
    int selectedDifficulty = EASY;

    int cards[MAX_CARDS] = { 0 };
    bool isFlipped[MAX_CARDS] = { false };
    int firstCardIndex = -1;
    int secondCardIndex = -1;
    int pairsFound = 0;
    int attempts = 0;
    int currentSelection = 0;
    int score = 0; // Score variable
    int timeLimit = 60; // Time limit in seconds
    sf::Clock clock; // Clock for timing
    int gameState = MAIN_MENU;
    int gridSize = 0; // Define gridSize here
    int cardCount = 0; // Define cardCount here

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (gameState == MAIN_MENU) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Up) {
                        selectedOption = (selectedOption - 1 + 2) % 2; // Wrap around
                    }
                    if (event.key.code == sf::Keyboard::Down) {
                        selectedOption = (selectedOption + 1) % 2; // Wrap around
                    }

                    if (event.key.code == sf::Keyboard::Enter) {
                        if (selectedOption == 0) {
                            gameState = DIFFICULTY_MENU;
                            selectedOption = 0; // Reset selection to Easy
                        }
                        else if (selectedOption == 1) {
                            window.close(); // Exit the game
                        }
                    }
                }
            }
            else if (gameState == DIFFICULTY_MENU) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Up) {
                        selectedOption = (selectedOption - 1 + 4) % 4; // Wrap around
                    }
                    if (event.key.code == sf::Keyboard::Down) {
                        selectedOption = (selectedOption + 1) % 4; // Wrap around
                    }

                    if (event.key.code == sf::Keyboard::Enter) {
                        if (selectedOption == 0) {
                            selectedDifficulty = EASY;
                            attempts = 6; // Set attempts for Easy (3x3 grid)
                        }
                        else if (selectedOption == 1) {
                            selectedDifficulty = MEDIUM;
                            attempts = 10; // Set attempts for Medium (4x4 grid)
                        }
                        else if (selectedOption == 2) {
                            selectedDifficulty = HARD;
                            attempts = 14; // Set attempts for Hard (5x5 grid)
                        }
                        else if (selectedOption == 3) {
                            window.close(); // Exit the game
                        }

                        // Start the game with the selected difficulty
                        gridSize = getGridSize(selectedDifficulty);
                        cardCount = (gridSize * gridSize) / 2;
                        int index = 0;

                        for (int i = 0; i < cardCount; ++i) {
                            cards[index++] = i; // Country
                            cards[index++] = i + cardCount; // Capital
                        }

                        // Shuffle cards using std::shuffle
                        std::random_device rd;
                        std::mt19937 eng(rd());
                        std::shuffle(cards, cards + index, eng);

                        std::fill(std::begin(isFlipped), std::end(isFlipped), false);
                        firstCardIndex = -1;
                        secondCardIndex = -1;
                        pairsFound = 0;
                        currentSelection = 0; // Reset current selection
                        score = 0; // Reset score
                        timeLimit = getTimeLimit(selectedDifficulty); // Set time limit based on difficulty
                        clock.restart(); // Restart the clock
                        gameState = IN_GAME; // Transition to the game state
                    }
                }
            }
            else if (gameState == IN_GAME) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Up) {
                        currentSelection = (currentSelection - gridSize + (gridSize * gridSize)) % (gridSize * gridSize); // Move up in the grid
                    }
                    if (event.key.code == sf::Keyboard::Down) {
                        currentSelection = (currentSelection + gridSize) % (gridSize * gridSize); // Move down in the grid
                    }
                    if (event.key.code == sf::Keyboard::Left) {
                        currentSelection = (currentSelection - 1 + (gridSize * gridSize)) % (gridSize * gridSize); // Move left in the grid
                    }
                    if (event.key.code == sf::Keyboard::Right) {
                        currentSelection = (currentSelection + 1) % (gridSize * gridSize); // Move right in the grid
                    }
                    if (event.key.code == sf::Keyboard::Enter) {
                        if (isFlipped[currentSelection]) {
                            continue; // Already flipped
                        }

                        isFlipped[currentSelection] = true;

                        if (firstCardIndex == -1) {
                            firstCardIndex = currentSelection; // First card flipped
                        }
                        else {
                            secondCardIndex = currentSelection; // Second card flipped
                            // Check for match
                            if (cards[firstCardIndex] == cards[secondCardIndex] - cardCount) {
                                pairsFound++;
                                score += 10; // Increment score on successful match
                                attempts++; // Gain an attempt on a successful match
                                if (pairsFound == cardCount) {
                                    cout << "You found all pairs!" << endl;
                                    gameState = MAIN_MENU; // Return to main menu after winning
                                }
                            }
                            else {
                                attempts--; // Decrease attempts on a failed match
                                // Delay to show the second card before flipping back
                                window.clear();
                                for (int i = 0; i < gridSize; ++i) {
                                    for (int j = 0; j < gridSize; ++j) {
                                        int idx = i * gridSize + j;
                                        float cardSize = getCardSizeAndPosition(gridSize, window).x;
                                        drawSquare(window, j * cardSize + PADDING * (j + 1), i * cardSize + PADDING * (i + 1), cardSize, sf::Color ::Black, sf::Color::Red);
                                        if (isFlipped[idx]) {
                                            text.setString(cards[idx] < cardCount ? countries[cards[idx]] : capitals[cards[idx] - cardCount]);
                                            text.setCharacterSize(16); // Smaller size for country/capital text
                                            text.setPosition(j * cardSize + cardSize / 4 + PADDING * (j + 1), i * cardSize + cardSize / 4 + PADDING * (i + 1));
                                            window.draw(text);
                                        }
                                    }
                                }
                                window.display();
                                sf::sleep(sf::seconds(1));
                                isFlipped[firstCardIndex] = false;
                                isFlipped[secondCardIndex] = false;
                            }
                            firstCardIndex = -1;
                            secondCardIndex = -1;
                        }

                        // Check if attempts are exhausted
                        if (attempts <= 0) {
                            cout << "No more attempts left! Returning to main menu." << endl;
                            gameState = GAME_OVER; // Return to game over state
                        }
                    }
                }
            }
        }

        // Clear the window
        window.clear();

        if (gameState == MAIN_MENU) {
            // Draw the main menu
            startText.setFillColor(selectedOption == 0 ? sf::Color::Red : sf::Color::White);
            difficultyText.setFillColor(sf::Color::White); // Not selectable in main menu
            window.draw(startText);
            window.draw(difficultyText);
        }
        else if (gameState == DIFFICULTY_MENU) {
            // Draw the difficulty selection menu
            easyText.setFillColor(selectedOption == 0 ? sf::Color::Red : sf::Color::White);
            mediumText.setFillColor(selectedOption == 1 ? sf::Color::Red : sf::Color::White);
            hardText.setFillColor(selectedOption == 2 ? sf::Color::Red : sf::Color::White);
            exitText.setFillColor(selectedOption == 3 ? sf::Color::Red : sf::Color::White);

            window.draw(difficultyText);
            window.draw(easyText);
            window.draw(mediumText);
            window.draw(hardText);
            window.draw(exitText);
        }
        else if (gameState == IN_GAME) {
            // Draw the game grid
            for (int i = 0; i < gridSize; ++i) {
                for (int j = 0; j < gridSize; ++j) {
                    int index = i * gridSize + j;
                    float cardSize = getCardSizeAndPosition(gridSize, window).x;
                    drawSquare(window, j * cardSize + PADDING * (j + 1), i * cardSize + PADDING * (i + 1), cardSize,
                        isFlipped[index] ? sf::Color::Red : sf::Color::Black,
                        (index == currentSelection) ? sf::Color::Red : sf::Color::Transparent); // Outline only if selected

                    if (isFlipped[index]) {
                        text.setString(cards[index] < cardCount ? countries[cards[index]] : capitals[cards[index] - cardCount]);
                        text.setCharacterSize(16); // Smaller size for country/capital text
                        text.setPosition(j * cardSize + cardSize / 4 + PADDING * (j + 1), i * cardSize + cardSize / 4 + PADDING * (i + 1));
                        window.draw(text);
                    }
                }
            }

            // Update and draw the attempt counter, score, and timer
            attemptText.setString("Attempts: " + to_string(attempts));
            window.draw(attemptText);

            scoreText.setString("Score: " + to_string(score));
            window.draw(scoreText);

            // Calculate remaining time
            int elapsedTime = static_cast<int>(clock.getElapsedTime().asSeconds());
            int remainingTime = timeLimit - elapsedTime;
            timerText.setString("Time: " + to_string(remainingTime));
            timerText.setPosition((window.getSize().x - timerText.getGlobalBounds().width) / 2, 30); // Centered position for timer
            window.draw(timerText);

            // Check if time is up
            if (remainingTime <= 0) {
                cout << "Time's up! Game Over!" << endl;
                gameState = GAME_OVER; // Transition to game over state
                window.draw(noMoreTimeText); // Display no more time message
            }
        }
        else if (gameState == GAME_OVER) {
            window.draw(gameOverText);
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Enter) {
                    gameState = MAIN_MENU; // Restart the game
                }
                else if (event.key.code == sf::Keyboard::Escape) {
                    window.close(); // Exit the game
                }
            }
            if (attempts <= 0) {
                window.draw(noMoreAttemptsText); // Display no more attempts message
            }
        }

        window.display();
    }

    return 0;
}