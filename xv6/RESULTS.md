# Results

## Setup
- Children: 3 CPU-bound processes
- Tickets: 10, 30, 60
- Duration per trial: 250, 700, 1200 ticks

## Workload
Each child sets its own ticket count with `settickets(n)` and then runs the same CPU-bound loop for a fixed wall-clock interval.
Work is measured as loop iterations completed in that interval.

## Results
Expected shares for tickets `10:30:60` are:
- 10 tickets -> 10.0%
- 30 tickets -> 30.0%
- 60 tickets -> 60.0%


| Trial | Duration (ticks) | Tickets | Expected (%) | Observed (%) |
|---|---:|---|---|---|
| 1 | 250  | 10 | 10 | 19.2 |
| 1 | 250  | 30 | 30 | 36.2 |
| 1 | 250  | 60 | 60 | 44.5 |
| 2 | 700  | 10 | 10 | 18.7 |
| 2 | 700  | 30 | 30 | 36.6 |
| 2 | 700  | 60 | 60 | 44.6 |
| 3 | 1200 | 10 | 10 | 18.1 |
| 3 | 1200 | 30 | 30 | 37.7 |
| 3 | 1200 | 60 | 60 | 44.1 |

## Notes on Variance and Why Longer Runs Convergence
- Lottery scheduling is probabilistic, so short runs can deviate from expected proportions.
- Over longer runs, the observed shares move closer to expected shares because we have more scheduling decisions that the lottery is averaging over, but what I noticed is that the numbers don't get better as much with longer runs.
