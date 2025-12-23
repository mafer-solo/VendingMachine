#include <iostream>     // Allows input/output (cin, cout)
#include <iomanip>      // Allows formatted output (setw, setprecision)
#include <string>       // Allows use of string type
#include <map>          // Stores products using key-value pairs
#include <vector>       // Allows dynamic list for shopping cart
#include <sstream>      // Allows formatting money output
#include <limits>       // Helps clear invalid input from user
#include <algorithm>    // Allows functions like find() and count()
 
using namespace std;     // Avoids writing std:: before every command

//Product structure: stores data from one vending item
struct Product {
    string code;           // Unique product code (H01, S01, etc.)
    string name;           // Name of the product (Chai latte, Quavers..)
    string category;       // Product Category ( Hot Drink, Snack. )
    int price;             // Price in pence (200 = 2.00)
    int stock;             // How many items are available
};

// Formats pence value into monetary string (GBP X.XX)
string formatMoney(int pence) {
    ostringstream out;       // Create string stream
    out << "GBP" << fixed << setprecision(2)           
        << (pence / 100.0);   // Convert pence to pounds
    return out.str();         // Return formatted money
}

// Class that represent the complete vending machine
class FerSnackVendingMachine {   
    map<string, Product> items;       // Stores all products by product code
    vector<string> cart;              // Stores product codes selected by the user 
    int balancePence = 0;             // Amount of money inserted by the user
    int collectedPence = 0;           // Total money collected by the machine
 
public:
// Constructor: adds all itrems to the vending machine
FerSnackVendingMachine() {
    addProduct({"H01", "Flat White", "Hot Drink", 150, 7});     
    addProduct({"H02", "Chai Latte", "Hot Drink", 130, 6});     
    addProduct({"C01", "Still Water", "Cold Drink", 100, 8});   
    addProduct({"C02", "Red Bull", "Cold Drink", 180, 9});      
    addProduct({"S01", "Quavers", "Snack",       140, 9});      
    addProduct({"S02", "Digestives", "Snack",      90, 6});
}      

// Adds a product to the inventory map
void addProduct(const Product& p) {
    items[p.code] = p;                    // Store product using code as the key    
}

// Display all available products
void showMenu() {
    cout << "\n--- Available Products ---\n";
    cout << left << setw(6) << "Code"
         << setw(15) << "Name"
         << setw(12) << "Category"
         << setw(8) << "Price"
         << "Stock\n";

    for (auto& entry : items) {                  // Loop through each product
        Product p = entry.second;                // Get product data
        cout << left << setw(6) << p.code     
             << setw(15) << p.name
             << setw(12) << p.category
             << setw(8) << formatMoney(p.price)
             << p.stock << "\n";      
    }

    cout << "Balance: " << formatMoney(balancePence) << "\n";  // Show insert money

}

// Allows the user to insert the money
void insertMoney() {
    double pounds;                          // User enter money in GBP
    cout << "Insert money (e.g. 1 or 1.50): ";
    cin >> pounds;                          // Read input

    // Validate numeric input 
    if (!cin) {                             // Handle invalid input
        cin.clear();                        // Clear error state
        cin.ignore(numeric_limits<streamsize>::max(), '\n');             // Remove invalid caracters
        cout << "Invalid input.\n";
        return;

    }
    
    int pence = static_cast<int>(pounds * 100 + 0.5);   // Convert to pence
    if (pence <= 0) {                                   
        cout << "Amount must be positive.\n";
        return;
    }

    balancePence += pence;                       // Add money to balance
    cout << " Balance updated: " 
         << formatMoney(balancePence) << "\n";

}
// Add item to cart (BUY PRODUCT)
void addItemToCart() {
    showMenu();                                // user sees products before choosing

    string code;                              // Store product code
    cout << "Enter product code to buy: ";
    cin >> code;

    // Check if product code exists
    if (!items.count(code)) {                   
        cout << "Invalid product code.\n" ;
        return;
    }

    Product& p = items[code];
   
    // Check stock availability
    if (p.stock <= 0) {                       // Ensure stock is available
        cout << "Item out of stock.\n";       // Print this message
        return;

    }

    p.stock--;                             // Reduce stock 
    cart.push_back(code);                  // Add item to cart

     // Show selected product and suggestion
    cout << "\nYou bought: " << p.name 
         << " (" << formatMoney(p.price) << ")\n";       
    
    showSuggestion(code);                              // Show suggestion based on item

}

// Shows suggestions depending on the selected item
void showSuggestion(const string& code) {
    if (code == "H01" || code == "H02")        
       cout << "Suggestion: Digestives taste great with hot drinks.\n";

    else if (code == "C01" || code == "C02")   
        cout << "Suggestion: Quavers go well with your cold drink.\n";
    
    else if (code == "S02")
        cout << "Suggestion: Try a Chai Latte with Digestives.\n";
    
}

// Calculates the total price before discounts
int calculateSubtotal() {
    int total = 0;                                 // This store the total
    for (string code : cart)                       // loop through cart
        total += items[code].price;                // Add price for each items
    return total;                                  // Return subtotal

}

// Calculates available discounts
int calculateDiscount() {
    int discount = 0;                             // Start with 0 discount

    // Hot drinl + Digestives discout

    if ((count(cart.begin(), cart.end(), "H01") +
         count(cart.begin(), cart.end(), "H02")) > 0 &&
        count(cart.begin(), cart.end(), "S02") > 0)
        discount += 15;
    return discount;
}

// Displays cart contents and pricing
void showCart() {
    if (cart.empty()) {                            
        cout << "Cart is empty.\n";
        return;

    }

    cout << "\n--- Shopping Cart --- \n";
    for (string code : cart)
        cout << items[code].name << "\n";


    int subtotal = calculateSubtotal();              // This calculates the total price of the item before discount
    int discount = calculateDiscount();             // This calculates the available discount
    
    cout << "Subtotal: " << formatMoney(subtotal) << "\n";
    cout << "Discount: " << formatMoney(discount) << "\n";
    cout << "Total: "    
         << formatMoney(subtotal - discount) << "\n";

}
// Handles checkout and dispenses items
void checkout() {
    if (cart.empty()) {
        cout << "Cart is empty.\n";
        return;
    }

    int total = calculateSubtotal() - calculateDiscount();
    showCart();

    // Check if user has enough money
    if (balancePence < total) {                        // The user must insert enough money to make the purchase
        cout << "Not enough balance.\n";
        return;
    }

    balancePence -= total;                               // This will deduct cost from the balance
    collectedPence += total;                            // This will add the money to the machine total

    cout << "Item dispensed successfully.\n";

    giveChange();                                        // Give change if needed
    cart.clear();                                        // This will empty the cart after purchase

}

// Returns change to the user
void giveChange() {
    if (balancePence > 0) {                             
        cout << "Change returned: "
             << formatMoney(balancePence) << "\n";                  // Formats pence value into moneytary string (GBP X.XX)
        balancePence = 0;
    
    }
}

// Main program loop: (menu system) 
void run() {                                             
    int choice;

        do {
            cout << "\n=== MENU ===\n"
                 << "1. Show products\n"
                 << "2. Insert money\n"
                 << "3. Buy product\n"
                 << "4. Show cart\n"
                 << "5. Checkout\n"
                 << "6. Exit\n"
                 << "Choose option: ";

            cin >> choice;                                          // Read user option

            if (!cin){
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
        
            switch (choice) {                                       
                case 1: showMenu(); break;                          // Display all available products
                case 2: insertMoney(); break;                       // Allows the user to insert the money                    
                case 3: addItemToCart(); break;
                case 4: showCart(); break;               
                case 5: checkout(); break;
                case 6: cout << "Thank you for using the vending machine.\n";
                        break;
                default: cout << "Invalid option.\n";
            }
        } while (choice != 6);

    }
};

// MAIN FUNCTION - PROGRAM STARTS HERE
int main() {
    cout << "Welcome to Fer Snack Vending Machine!\n";      // Welcome message
    FerSnackVendingMachine machine;                        // Create vending machine
    machine.run();                                         // Start program loop
    return 0;                                              // End program

}
