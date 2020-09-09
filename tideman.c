#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

// Function prototype - added by me
bool loop_check(int winner, int loser);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < (candidate_count - 1); i++)
    {
        for (int j = (i + 1); j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]]++;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            int pref_one = preferences[i][j];
            int pref_two = preferences[j][i];

            if (pref_one > pref_two)
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    //initiate bubble sort - need to iterate once for every pair
    for (int j = 0; j < pair_count; j++)
    {
        for (int i = 0; i < pair_count - 1; i++)
        {
            int spot = preferences[pairs[i].winner][pairs[i].loser];
            int next_spot = preferences[pairs[i + 1].winner][pairs[i + 1].loser];

            //compare current pair to next pair and swap if current pair margin of victory is lower than the next pair
            if (spot < next_spot)
            {
                pair holder = pairs[i];
                pairs[i] = pairs[i + 1];
                pairs [i + 1] = holder;
            }
        }
        return;
    }
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        //check to see if locking pair will create a loop
        if (loop_check(pairs[i].winner, pairs[i].loser))
        {
            locked[pairs[i].winner][pairs[i].loser] = true; //lock pair if it will not create a loop
        }
    }
    return;
}

//semi recursive function to check to see if creating an "edge"
//(arrow indicating a candidate is prefered over another) would create a loop
bool loop_check(int winner, int loser)
{
    bool check = true;
    int f_count = 0;

    for (int i = 0; i < candidate_count; i++)
    {
        if (locked[loser][i] == false) //check to see if "locked [loser][i] is false
        {
            f_count++; // if it is false, keep track of how many are false
            if (f_count == candidate_count) //if the entire row for that candidate is false,then there is no way a loop could be formed
            {
                return true;
            }
        }

        if (locked[loser][i] == true) //if "locked[loser][i]" is true
        {
            if (winner == i) //check to see if the loser points to the winner note this is important for recursion
            {
                return false; //return false if winner is pointed as this would create a loop
            }
            else
            {
                check = loop_check(winner, i); //if loser is pointing to someone other than the winner
            }                                  //check to see if that loser points to the winner
        }
    }
    return check;
}

// Print the winner of the election
void print_winner(void)
{
    int f_count = 0;

    for (int i = 0; i < candidate_count; i++) //itereate through each candidate column
    {
        f_count = 0;

        for (int j = 0; j < candidate_count; j++) //iterate through each candidate row
        {
            if (locked[j][i] == true) //if a candidate's row contains a true then that candidate cannot be the origin
            {
                break;
            }
            else
            {
                f_count++;
                if (f_count == candidate_count) //if candidates entire column is "false" then that candidate is the source and winner
                {
                    printf("%s\n", candidates[i]);
                }
            }
        }
    }
    return;
}

