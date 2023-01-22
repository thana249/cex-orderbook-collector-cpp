# Order Book Data Collector

## Description
This service collects order book data from the Binance and Bitkub cryptocurrency exchanges and saves the data to a file.

## Built With
- C++
- CMake
- Docker
- Docker Compose

## Prerequisites
- Docker
- Docker Compose

## Installation
1. Clone the repository: `git clone https://gitlab.com/thana249/orderbook_collector`
2. Build the Docker image: `docker-compose build`
3. Start the service: `docker-compose up`

## Usage
The service will automatically collect and save the order book data to a file named "ticker_timestamp.txt" in the project output directory.

## Configuration
You can configure the service by editing the `config.json` file in the config directory.

## Contributing
1. Fork the repository
2. Create your feature branch: `git checkout -b new-feature`
3. Commit your changes: `git commit -am 'Add some feature'`
4. Push to the branch: `git push origin new-feature`
5. Submit a pull request

## License
This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
