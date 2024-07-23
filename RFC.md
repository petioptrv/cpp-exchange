# Exchange Requirements

- API
  - Submit order
    - Limit
    - Market
  - Cancel order
  - Get order book snapshot
  - Order book changes stream
  - Trades stream
- Engine
  - Order matching
    - Order matched in sequence of arrival
    - Exchange fees for takers and makers

# Exchange Design

- Ticker symbol
  - Associated book
- User data
  - Placing orders must be checked against user budget and available stocks

# Exchange Optimization Experiments

- CPU pinning of the matching engine (Linux)

# Simulation Requirements

- Multiple traders of multiple types
  - Market makers
    - If alone on a ticker, get reference price from Alpaca (NBBO?)
  - Manual traders
    - Lots of time between actions
    - Somewhat random actions
      - Must still have underlying patterns
        - E.g. a probability of event (e.g. new event) that exerts pressure on the trader in one direction
  - Automated traders
    - Simple mean-reversal strategies
