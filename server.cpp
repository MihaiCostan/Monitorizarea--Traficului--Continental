#include <iostream>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <fstream>

#include "json.hpp"

using json = nlohmann::json;

void proceseaza_mesaj(std::string raw_data)
{
    // Transformam string-ul primit prin socket in obiect JSON
    auto j = json::parse(raw_data);

    if (j["type"] == "speed_update")
    {
        std::cout << "Masina " << j["car_id"] << " are viteza: " << j["value"] << " km/h\n";
    }
}

int main() {
    
    return 0;
}