
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <iterator>
#include <exception>
using namespace std;


bool testMode = true;

int dateSize = 0;

bool backIncldued = true;
bool forwardIncluded = true;

float oppositeTrendFactor = 0.5;

int stopIndexes[] = {200, 201, 224};

void debugStopIndex(int index) {
	for (int i = 0; i < (sizeof(stopIndexes) / sizeof(stopIndexes[0])); ++i) {
		if (index == stopIndexes[i]) {
			cout << "stop here" << endl;
		}
	}
}

struct MergedLine {
	float high;
	float low;
	int low_index;
	int high_index;
	int direction;
};

struct Pen {
	int direction;
	int start_index;
	int end_index;
	float high;
	float low;
};

struct ConfirmPenResult {
	bool confirmed;
	int new_high_index;
	int new_low_index;
	float new_high;
	float new_low;
};

struct SearchConsecutive2PensExistResult {
	int high_index;
	int low_index;
	float high;
	float low;
};

Pen testPen = {
	0, 0, 0, 0, 0
};

void PrintMergedLines(vector<MergedLine> &mergedLines, vector<Pen> &confirmedPen) {
    map<int, int> topMap, bottomMap;
    for (int i = 0; i < confirmedPen.size(); ++i) {
        Pen pen = confirmedPen[i];
        topMap.insert(pair<int, int>((pen.direction > 0) ? pen.end_index : pen.start_index, 1));
        bottomMap.insert(pair<int, int>((pen.direction > 0) ? pen.start_index : pen.end_index, 1));
    }
    
	cout << "\n####### Merged lines: ########\n";
	float highest = 0;
	float lowest = mergedLines[0].low;
	for (int i = 0; i < mergedLines.size(); ++i) {
		MergedLine cur = mergedLines[i];
		if (cur.high > highest) {
			highest = cur.high;
		}

		if (cur.low < lowest) {
			lowest = cur.low;
		}
	}

	for (int h = highest + 2; h >= lowest - 5; --h) {
		for (int i = 0; i < mergedLines.size(); ++i) {
			MergedLine cur = mergedLines[i];
			if (h == lowest - 5) {
				cout << " " << i << "\t";
				continue; 
			}
			if (h >= cur.low && h <= cur.high) {
			    if (topMap.find(i) != topMap.end() && cur.high == h) {
			        cout << "[顶]\t";
			    } else if (bottomMap.find(i) != bottomMap.end() && cur.low == h) {
			        cout << "[底]\t";
			    } else
				    cout << " *\t";
			} else {
				cout << "\t";
			}
		}
		cout << "\n";
	}
}

void PrintInput(float* high, float* low) {
	cout << "\n####### input data: ########\n";
	// int size = 6;
	float highest = 0;
	float lowest = low[0];
	for (int i = 0; i < dateSize; ++i) {
		if (high[i] > highest) {
			highest = high[i];
		}

		if (low[i] < lowest) {
			lowest = low[i];
		}
	}

	for (int h = highest + 2; h >= lowest - 5; --h) {
		for (int i = 0; i < dateSize; ++i) {
			if (h == lowest - 5) {
				cout << " " << i << "\t";
				continue; 
			}

			if (h >= low[i] && h <= high[i]) {
				cout << "\t*";
			} else {
				cout << "\t";
			}
		}
		cout << "\n";
	}
}

bool isHighLowEquals(MergedLine ml) {
	return ml.high == ml.low;
}

void generateMergedLines(int DataLen, float* highInput, float* lowInput, vector<MergedLine> &mergedLines) {
	// Deal with inital direction
	int direction = (highInput[0] > highInput[1]) ? -1 : 1;
	
	// Generate merged lines.
	for (int i = 1; i < DataLen; i++) {
		MergedLine prevML = mergedLines.back();
		MergedLine curML = {
			highInput[i],
			lowInput[i],
			i,
			i,
			direction
		};

		// Prev contains cur ML.
		if (prevML.high >= curML.high && prevML.low <= curML.low) {
			// 一字板
			if (isHighLowEquals(curML)) {
				continue;
			}
			if (direction > 0) {
				prevML.low = curML.low;
				prevML.low_index = i;
			} else {
				prevML.high = curML.high;
				prevML.high_index = i;
			}
			mergedLines[mergedLines.size() - 1] = prevML;
		} else if (prevML.high <= curML.high && prevML.low >= curML.low) {
			// Cur contain prev ML.

			// 一字板
			if (isHighLowEquals(prevML)) {
				mergedLines[mergedLines.size() - 1] = curML;
				continue;
			}
			if (direction > 0) {
				prevML.high = curML.high;
				prevML.high_index = i;
			} else {
				prevML.low = curML.low;
				prevML.low_index = i;
			}
			mergedLines[mergedLines.size() - 1] = prevML;
		} else {
			// Not included.
			if (curML.high >= prevML.high) {
				direction = 1;
			} else {
				direction = -1;
			}
			curML.direction = direction;
			mergedLines.push_back(curML);
		}
	}

	// Set direction for MergedLine
	for (int i = 0; i < mergedLines.size() - 1; ++i) {
		mergedLines[i].direction = (mergedLines[i + 1].high >= mergedLines[i].high) ? 1 : -1; 
	}
}

// Check cur ML is the top or bottom
// Return 1 as top, -1 as bottom. 0 as invalid, need to check.
int isTopOrBottom(MergedLine &cur, MergedLine &prev, MergedLine &next) {
	if (cur.high > prev.high && cur.high > next.high) {
		return 1;
	}

	if (cur.low < prev.low && cur.low < next.low) {
		return -1;
	}

	return 0;
}

// Check cur ML is the top or bottom
// Return 1 as top, -1 as bottom. 0 as invalid, need to check.
int isTopOrBottom(int curIndex, vector<MergedLine> &mergedLines, int DataLen) {
	if (curIndex >= DataLen - 1 || curIndex == 0) {
		return 0;
	}
	MergedLine curML = mergedLines[curIndex];
	MergedLine prevML = mergedLines[curIndex - 1];
	MergedLine nextML = mergedLines[curIndex + 1];

	return isTopOrBottom(curML, prevML, nextML);
}

// Check start and end ML is able to generate a Pen
// Return 1 as UP Pen, -1 as DOWN Pen. 0 as unable to generate.
int isAbleToGeneratePen(int startIndex, int endIndex, vector<MergedLine> &mergedLines, Pen previousPen, int DataLen) {
	int startTopBottom = isTopOrBottom(startIndex, mergedLines, DataLen);
	int endTopBottom = isTopOrBottom(endIndex, mergedLines, DataLen);

	if ((startTopBottom == 0 || endTopBottom == 0) || 
		(startTopBottom == endTopBottom)) {
		return 0;
	}

	// 1: UP; -1: DOWN
	int direction = (startTopBottom > 0) ? -1 : 1;
	
	// TODO: 缺口
	// Only when it is opposite trend
	//   1) If direction is DOWN, and the first ML's low < previous Pen's low.
	//   2) If direction is UP, and the first ML's high > previous Pen's high.
	MergedLine startML = mergedLines[startIndex];
	MergedLine startNextML = mergedLines[startIndex + 1];
	if (direction > 0) {
		// It is opposite trend
		if (startML.high < startNextML.low) {
			if (previousPen.high < startNextML.high) {
				float previousPenVal = abs(previousPen.high - previousPen.low);
				if ((startNextML.low - previousPen.low) > (previousPenVal * oppositeTrendFactor)) {
					return direction;
				}
				return direction;
			}
		}
	} else {
		// It is opposite trend
		if (startML.low > startNextML.high) {
			if (previousPen.low > startNextML.low) {
				float previousPenVal = abs(previousPen.high - previousPen.low);
				if ((previousPen.high - startNextML.high) > (previousPenVal * oppositeTrendFactor)) {
					return direction;
				}
			}
		}
	}

	// There must be more than 4 points betweew top/bottom points.
	if (endIndex - startIndex >= 4) {
		MergedLine startML = mergedLines[startIndex];
		MergedLine endML = mergedLines[endIndex];
		// Back included judgement.
		if (backIncldued) {
			MergedLine endRightML = mergedLines[endIndex + 1];
			// If opposite trend exist
			// if (direction > 0 && )

			// If UP, and the endML's right ML'low < start.low, included.
			if (direction > 0 && endRightML.low < startML.low) {
				return 0;
			} 

			// If DOWN, and the endML's right ML'high > start.high, included.
			if (direction < 0 && endRightML.high > startML.high) {
				return 0;
			}
		}
		
		// Forward included judgement.
		if (forwardIncluded) {
			MergedLine startLeftML = mergedLines[startIndex - 1];
			// If UP, and the startML's left ML'high > endML.high, included. 
			if (direction > 0 && startLeftML.high > endML.high) {
				return 0;
			}

			// If DOWN, and the startML's left ML'low < endML.low, included. 
			if (direction < 0 && startLeftML.low < endML.low) {
				return 0;
			}
		}

		return direction;
	}

	return 0;
}

Pen initializePen(vector<MergedLine> &mergedLines, int DataLen) {
	if (!testMode) {
		float highest = 0, lowest = 999999;
		int high = 0, low = 0;
		for (int i = 0; i < DataLen && i < 25; ++i) {
			MergedLine cur = mergedLines[i];
			if (cur.high > highest) {
				highest = cur.high;
				high = i;
			}
			if (cur.low < lowest) {
				lowest = cur.low;
				low = i;
			}
		}

		return {
			(high > low) ? 1 : -1,
			(high > low) ? low : high,
			(high > low) ? high : low,
			highest,
			lowest
		};
	}

	int index = 1;
	while (index < DataLen) {
		int topBottom = isTopOrBottom(index, mergedLines, DataLen);
		if (topBottom != 0) {
			return {
				topBottom,
				0,
				index,
				(topBottom > 0) ? mergedLines[index].high : mergedLines[0].high,
				(topBottom > 0) ? mergedLines[0].low : mergedLines[index].low
			};
		}
		index++;
	}
	return {0, 0, 0, 0, 0};
}

// Main dfs logic
ConfirmPenResult dfsGeneratePen(Pen &rootPen, vector<MergedLine> &mergedLines, int index, int DataLen, vector<Pen> &confirmedRes) {
	// Recursion ends.
	if (index >= DataLen - 1) {
		return {
			// Confirmed
			true, 0, 0, 0, 0
		};
	}

	debugStopIndex(index);

	// If there is no Pen generated yet. Initialze the Pen first.
	if (rootPen.direction == 0 && rootPen.high == 0 && rootPen.low == 0) {
		rootPen = initializePen(mergedLines, DataLen);
		index = rootPen.end_index;
	}

	MergedLine initialML = mergedLines[index];
	int highest_index = 0, lowest_index = 0;
	float highest = -1, lowest = 99999999;
	int curIndex = index + 1;

	// Pen's direction is DOWN, so next direction should be UP
	if (rootPen.direction < 0) {
		// The highest that unable to generate a UP pen
		float unabledHighest = -1;
		int unabledHighIndex = -1;

		while (curIndex < DataLen) {
			MergedLine curML = mergedLines[curIndex];
			if (curML.high > highest) {
				highest = curML.high;
				highest_index = curIndex;
			}
			if (curML.low < lowest) {
				lowest = curML.low;
				lowest_index = curIndex;
			}

			debugStopIndex(curIndex);

			// If the lowest(hasn't be a Pen) is lower than rootPen's low, which means the rootPen's direction(DOWN)
			// could be expended. So it is incorrect. Return false.
			if (lowest < rootPen.low) {
				return {
					false,	// NOT confirmed
					highest_index,
					lowest_index,
					highest,
					lowest
				};
			}

			// If the curML's high is lower than highest, it could not be the next TOP ML, continue.
			if (curML.high < highest) {
				curIndex++;
				continue;
			}
			
			// If cur is the TOP.
			if (isAbleToGeneratePen(index, curIndex, mergedLines, rootPen, DataLen) > 0) {
				Pen nextPen = {
					1,	// New pen direction: UP
					index,
					curIndex,
					curML.high,
					initialML.low
				};

				ConfirmPenResult res = dfsGeneratePen(nextPen, mergedLines, curIndex, DataLen, confirmedRes);
				
				// If res confirmed, return
				if (res.confirmed) {
					// cout << "\n res confirmed! cur index: " << curIndex << " ;\n";
					confirmedRes.push_back(nextPen);
					return res;
				} 
				// If root Pen's low is higher than new low, it means cur Pen is not correct,
				// Since it is able to be lower by its direction(DOWN). So we need to return 
				// back the res, let the upper layer dfs to deal with it.
				// TODO: when equals
				else if (rootPen.low > res.new_low) {
					float newhigh, newLow;
					int tmpHighIndex, tmpLowIndex;
					// new high & index depends on cur hight ML(curML.high) and res.new_high
					if (curML.high >= res.new_high) {
						newhigh = curML.high;
						tmpHighIndex = curIndex;
					} else {
						newhigh = res.new_high;
						tmpHighIndex = res.new_high_index;
					}

					newLow = res.new_low;
					tmpLowIndex = res.new_low_index;

					return {
						false,	// NOT confirmed
						tmpHighIndex, tmpLowIndex, newhigh, newLow
					};
				} 
				// If there is new high from the res, if means cur next Pen(curML)'s high should changed to that
				// and continue. So we need to change the curIndex to the new_high_index to create new 
				// Pen and continue;
				else if (nextPen.high < res.new_high) {
						curIndex = res.new_high_index;
						continue;
				} else {
					// cout << "\n\n[ERROR] no valid res validated! ##########################";
					// cout << "\ncur index: " << curIndex;
					// cout << "\n\n";
					curIndex++;
				}
			} else {
				// If could not generate pen but the highest is higher than rootPen's high, return false.
				if (highest > rootPen.high && highest > unabledHighest) {
					unabledHighest = highest;
					unabledHighIndex = curIndex;
				}

				// If is not TOP or Bottom, continue
				curIndex++;
			}
		}

		// If unable highest exist, return false.
		if (unabledHighIndex > 0) {
			return {
				false,	// NOT confirmed
				highest_index,
				lowest_index,
				highest,
				lowest
			};
		}

		// TODO: deal with end case
		return {true, 0, 0, 0, 0};
	}
	// Pen's direction is UP, so the next Pen's direction should be DOWN
	else if (rootPen.direction > 0) {
		// The lowest that unable to generate a DOWN pen
		float unabledLowest = 9999999;
		int unabledLowIndex = -1;

		while (curIndex < DataLen) {
			MergedLine curML = mergedLines[curIndex];
			if (curML.high > highest) {
				highest = curML.high;
				highest_index = curIndex;
			}
			if (curML.low < lowest) {
				lowest = curML.low;
				lowest_index = curIndex;
			}

			debugStopIndex(curIndex);

			// If the highest(hasn't be a Pen) is higher than rootPen's high, which means the rootPen's direction(UP)
			// could be expended. So it is incorrect. Return false.
			if (highest > rootPen.high) {
				return {
					false,	// NOT confirmed
					highest_index,
					lowest_index,
					highest,
					lowest
				};
			}

			// If the curML's low is higher than lowest, it could not be the next BOTTOM ML, continue.
			if (curML.low > lowest) {
				curIndex++;
				continue;
			}
			
			// If cur is the BOTTOM.
			if (isAbleToGeneratePen(index, curIndex, mergedLines, rootPen, DataLen) < 0) {
				Pen nextPen = {
					-1,	// New pen direction: DOWN
					index,
					curIndex,
					initialML.high,
					curML.low
				};

				ConfirmPenResult res = dfsGeneratePen(nextPen, mergedLines, curIndex, DataLen, confirmedRes);
				
				// If res confirmed, return
				if (res.confirmed) {
					// cout << "\n res confirmed! cur index: " << curIndex << " ;\n";
					confirmedRes.push_back(nextPen);
					return res;
				} 
				// If root Pen's high is lower than new high, it means root Pen is not correct,
				// Since it is able to be higher by its direction(UP). So we need to return 
				// back the res, let the upper layer dfs to deal with it.
				// TODO: when equals
				else if (rootPen.high < res.new_high) {
					int newHighIndex, newLowIndex;
					float newHigh, newLow;
					
					newHigh = res.new_high;
					newHighIndex = res.new_high_index;

					// new low & index depends on Pen's low(rootPen.low) and res.new_low
					if (rootPen.low <= res.new_low) {
						newLow = rootPen.low;
						newLowIndex = index;
					} else {
						newLow = res.new_low;
						newLowIndex = res.new_low_index;
					}

					return {
						false,	// NOT confirmed
						newHighIndex, newLowIndex, newHigh, newLow
					};
				} 
				// If there is new low from the res, if means new next Pen(curML)'s low should changed to that
				// and continue. So we need to change the curIndex to the new_high_index to create new next  
				// Pen and continue;
				else if (nextPen.low > res.new_low) {
					curIndex = res.new_low_index;
					continue;
				} else {
					// cout << "\n\n[ERROR] no valid res validated! ##########################";
					// cout << "\ncur dir: " << rootPen.direction << "cur index: " << curIndex;
					// cout << "\n\n";
					curIndex++;
				}
			} else {
				// If could not generate pen but the lowest is lower than rootPen's low, return false.
				if (lowest < rootPen.low && lowest < unabledLowest) {
					unabledLowest = lowest;
					unabledLowIndex = curIndex;
				}

				// If is not TOP or Bottom, continue
				curIndex++;
			}
		}

		// If unable lowest exist, return false.
		if (unabledLowIndex > 0) {
			return {
				false,	// NOT confirmed
				highest_index,
				lowest_index,
				highest,
				lowest
			};
		}

		// TODO: deal with end case
		return {true, 0, 0, 0, 0};
	} else {
		cout << "\n\n[ERROR] Root pen's direction is 0! ############## \n\n";
	}
}


ConfirmPenResult generatePen(vector<MergedLine> &mergedLines, int DataLen, vector<Pen> &confirmedRes) {
	int index = 1, previousIndex = 0;
	while (index < DataLen) {
		int topBottom = isTopOrBottom(index, mergedLines, DataLen);
		if (topBottom != 0) {
			Pen rootPen = {
				topBottom,
				previousIndex,
				index,
				(topBottom > 0) ? mergedLines[index].high : mergedLines[0].high,
				(topBottom > 0) ? mergedLines[0].low : mergedLines[index].low
			};
			vector<Pen> newRes;
			confirmedRes = newRes;
			ConfirmPenResult res = dfsGeneratePen(rootPen, mergedLines, rootPen.end_index, DataLen, confirmedRes);
			if (res.confirmed) {
				return res;
			}
			previousIndex = index;
		}
		index++;
	}
	return {0, 0, 0, 0, 0};
}


void ChanLun1(int DataLen, float* pfOUT, float* highInput, float* lowInput, float* closeInput)
{
	cout << "\n\n************ Chan lun start: **************\n\n";

	if (testMode) {
		// PrintInput(highInput, lowInput);
	}

	vector<MergedLine> mergedLines;
	// TODO: start with high == low
	mergedLines.push_back({
		highInput[0],
		lowInput[0],
		0,
		0,
		0
	});


	generateMergedLines(DataLen, highInput, lowInput, mergedLines);

	if (testMode) {
		int nextLine = 100;
		cout << "\n\n Merged lines: " << endl;
		cout << "high: " << endl;
		for (int i = 0; i < mergedLines.size(); ++i) {
			cout << mergedLines[i].high << ", ";
			if (i % nextLine == nextLine - 1) {
				cout << endl;
			}
		}

		cout << "\n\nlow: " << endl;
		for (int i = 0; i < mergedLines.size(); ++i) {
			cout << mergedLines[i].low << ", ";
			if (i % nextLine == nextLine - 1) {
				cout << endl;
			}
		}
		cout << "\n\n";
	}


    vector<Pen> confirmedPens;
	Pen initialPen = {
		0, 0, 0, 0, 0
	};

	try {
		// ConfirmPenResult res = dfsGeneratePen(initialPen, mergedLines, 0, mergedLines.size(), confirmedPens);
		ConfirmPenResult res = generatePen(mergedLines, mergedLines.size(), confirmedPens);
	} catch (exception &e) {
		cout << e.what() << endl;
		// Initialize output array as 0
		for (int i = 0; i < DataLen; ++i) {
			pfOUT[i] = highInput[i];
		}
		return;
	}

	if (testMode) {	
		for (int i = 0; i < confirmedPens.size(); ++i) {
		    cout << "\nconfirm res " << i << " , direction " << confirmedPens[i].direction << " , start index is: " << confirmedPens[i].start_index << "  end index is: " << confirmedPens[i].end_index;
			cout << " , high val: " << confirmedPens[i].high << " , low val: " << confirmedPens[i].low << "\n\n";
		}

		// PrintMergedLines(mergedLines, confirmedPens);
	}
	
	// Initialize output array as 0
	for (int i = 0; i < DataLen; ++i) {
		pfOUT[i] = 0;
	}

	// Setup real low high output. 1 as low, 2 as high, 5 as error
    for (int i = 0; i < confirmedPens.size(); ++i) {
        Pen pen = confirmedPens[i];
		
		if (pen.direction > 0) {
			pfOUT[mergedLines[pen.start_index].low_index] = 1;
			pfOUT[mergedLines[pen.end_index].high_index] = 2;
		} else if (pen.direction < 0) {
			pfOUT[mergedLines[pen.start_index].high_index] = 2;
			pfOUT[mergedLines[pen.end_index].low_index] = 1;
		} else {
			pfOUT[pen.start_index] = 5;
			pfOUT[pen.end_index] = 5;
		}
    }

	if (testMode) {
		cout << "\n\nconfirmed pens index:" << endl;
		for (int i = confirmedPens.size() - 1; i >= 0; --i) {
			cout << confirmedPens[i].start_index << ", ";
		}
		cout << confirmedPens[0].end_index << endl;

		cout << "\n\npfOUT index: " << endl;
		for (int i = 0; i < DataLen; ++i) {
			if (pfOUT[i] != 0) {
				cout << i << ", ";
			}
		}
		cout << "\n";
	}
}


void testChanLun(string info, float high[], float low[], int DataLen) {
	cout << "\n\n\n#########  " << info << "  #########\n\n";

	float res[100] = {};
	float close[] 	= 	{5, 4, 3, 2, 5};

	float *output = &res[0], *in1 = &high[0], *in2 = &low[0], *in3 = &close[0];

	testPen = {
		1,	// dir
		0,	// start idx
		1,	// end idx
		13,	// high
		10	// low
	};

	// size = sizeof(high)/sizeof(high[0]);
	dateSize = DataLen;

	ChanLun1(dateSize, output, in1, in2, in3);
}


int main() {
	// // TEST 1:
	// float high[] 	= 	{10,	8, 	4, 	11, 12, 8, 15, 12};
	// float low[] 	= 	{2,		3, 	4, 	8, 	6, 	3, 15, 11};
	// testChanLun("TEST 1", high, low, sizeof(high)/sizeof(high[0]));


	// TEST 2:
	// float high2[] 	= 	{13, 14, 12, 12, 11, 10, 9, 10, 11, 12, 15, 12, 13, 12, 10, 11, 16, 17, 16, 6, 8};
	// float low2[] 	= 	{10, 11, 10,  9,  8,  7, 6,  7,  8,  9, 11, 10, 11,  8,  5,  9, 10, 15,  4, 3, 5};
	
	// testChanLun("TEST 2", high2, low2, sizeof(high2)/sizeof(high2[0]));



	// // TEST 3:
	// float high3[] 	= 	{13, 14, 12, 11, 10, 9, 10, 11, 12, 15, 12, 13, 12, 10, 11};
	// float low3[] 	= 	{10, 11,  9,  8,  7, 6,  7,  8,  9, 11, 10, 11,  8,  5,  9};
	
	// testChanLun("TEST 3", high3, low3, sizeof(high3)/sizeof(high3[0]));


	// // TEST 4:
	// float high4[] 	= 	{10, 13, 14, 12, 11, 10, 9, 10, 11, 12, 15, 12, 13, 12, 17, 11, 10, 9, 8, 9};
	// float low4[] 	= 	{ 8, 10, 11,  9,  8,  7, 4,  7,  8,  9, 11, 10, 11,  8, 10,  9,  8, 7, 6, 8};
	
	// testChanLun("TEST 4", high4, low4, sizeof(high4)/sizeof(high4[0]));


	// // TEST 5:
	// float high5[] 	= 	{13, 14, 12, 11, 10, 9, 16, 8, 12, 15, 12, 13, 12, 10, 11};
	// float low5[] 	= 	{10, 11,  9,  8,  7, 6, 12, 4,  9, 11, 10, 11,  8,  5,  9};
	
	// testChanLun("TEST 5, opposite trend", high5, low5, sizeof(high5)/sizeof(high5[0]));

	// // TEST 6:
	// float high6[] 	= 	{11, 13, 12, 11, 10, 9, 10, 11, 12, 13, 14, 12, 13, 8, 9, 11, 15, 14};
	// float low6[] 	= 	{8, 10, 9, 8, 7, 6, 7, 8, 9, 10, 11, 10, 11, 5, 6, 9, 10, 8};
	
	// testChanLun("TEST 6, opposite trend", high6, low6, sizeof(high6)/sizeof(high6[0]));

   	// // TEST 7:
	// float high7[] 	= 	{11, 13, 12, 11, 10, 9, 10, 11, 12, 13, 14,  8, 9, 11, 15, 14};
	// float low7[] 	= 	{8,  10, 9,  8,   7, 6, 7,   8, 9,  10, 11,  5, 6, 9,  10, 8};
	
	// testChanLun("TEST 7, opposite trend", high7, low7, sizeof(high7)/sizeof(high7[0]));


   	// // TEST 8:
	// float high8[] 	= 	{11, 13, 12, 11, 10, 9, 10, 11, 17, 19, 18, 17, 15, 14, 16, 17};
	// float low8[] 	= 	{8,  10, 9,  8,   7, 6, 7,   8, 9,  17, 16, 14, 13, 12, 14, 15};
	
	// testChanLun("TEST 8, forward included", high8, low8, sizeof(high8)/sizeof(high8[0]));
	
	
   	// // TEST 9:
	// float high9[] 	= 	{ 8, 9, 10, 9, 8, 7, 6, 9, 10, 11, 12, 11, 10, 9, 8, 14, 8, 11, 9};
	// float low9[] 	= 	{ 7, 8,  9, 8, 7, 6, 4, 8,  9, 10, 11,  9,  8, 7, 6,  7, 6,  9, 8};
	
	// testChanLun("TEST 9, back included", high9, low9, sizeof(high9)/sizeof(high9[0]));


	// // Test 10:
	// float realHigh1[] 	= 	{ 2200, 2444.80, 2300, 2200, 2100, 2068.86, 2100, 2150, 2200, 2270.27, 2248, 2223, 2176, 2228, 2242, 2230, 2163, 2166, 2153, 2211, 2234, 2260, 2250, 2201, 2111, 2120, 2078, 2034, 2060, 2051, 2044, 2122, 2177, 2099, 2079, 2042, 2052, 2079, 2066, 2146, 2134};
	// float realLow1[] 	= 	{ 1900, 2308.76, 2100, 2000, 1900, 1849.65, 2000, 2050, 2100, 2151.35, 2172, 2149, 2165, 2161, 2183, 2122, 2093, 2103, 2078, 2143, 2176, 2172, 2183, 2082, 2068, 2075, 2008, 2000, 1984, 2026, 2014, 2049, 2098, 2014, 2030, 1974, 1986, 2035, 2024, 2052, 2081};
	
	// testChanLun("TEST 10, real data test", realHigh1, realLow1, sizeof(realHigh1)/sizeof(realHigh1[0]));


	// Test 11:
	float realHigh2[] 	= 	{4267.82, 4268.14, 4268.1, 4269.13, 4269.66, 4270.64, 4271.33, 4271.73, 4271.66, 4271.06, 4271.18, 4271.38, 4271.77, 4271.81, 4271.78, 4271.22, 4271.15, 4271, 4271.47, 4271.79, 4272.77, 4273.14, 4274.27, 4274.23, 4274.15, 4275.32, 4264.48, 4261.18, 4260.13, 4258.67, 4256.65, 4258.64, 4258.61, 4258.34, 4256.66, 4254.89, 4251.04, 4252.48, 4254.77, 4255.5, 4255.66, 4256.94, 4258.45, 4259.39, 4258.75, 4256.82, 4257.18, 4255.24, 4255.06, 4256.34, 4261.14, 4262.11, 4261.52, 4262.32, 4262.28, 4262.23, 4261.05, 4256.46, 4256.28, 4256.52, 4256.79, 4257.36, 4258.85, 4259.49, 4258.58, 4256.65, 4256.3, 4254.96, 4254.79, 4255.25, 4255.96, 4256.02, 4255, 4256.43, 4259.42, 4260.24, 4260.54, 4261.02, 4261.44, 4264.37, 4265.78, 4265.71, 4264.43, 4263.85, 4263.15, 4262.9, 4261.97, 4260.23, 4260.38, 4260.1, 4260.49, 4261.07, 4261.28, 4261.77, 4261.64, 4259.89, 4258.45, 4256.66, 4256.42, 4256.44, 4255.52, 4255.03, 4255.03, 4255.04, 4255.57, 4257, 4257, 4255.66, 4255.59, 4256.04, 4256.01, 4255.8, 4255.79, 4255.49, 4254.82, 4254.26, 4252.71, 4249.88, 4250.05, 4250.08, 4249.87, 4250.19, 4251.9, 4252.39, 4252.48, 4252.21, 4254.02, 4254.61, 4255.44, 4254.95, 4254.85, 4254.75, 4255.53, 4256.58, 4259.75, 4261.43, 4261.66, 4261.24, 4261.03, 4260.74, 4262.27, 4264.86, 4265.72, 4267.21, 4267.82, 4268.41, 4271.05, 4269.15, 4268.54, 4267.9, 4268.6, 4268.79, 4268.12, 4267.43, 4269.09, 4270.28, 4272.35, 4273.44, 4273.88, 4273.03, 4272.17, 4272.12, 4272.01, 4271.99, 4271.85, 4271.57, 4270.99, 4269.14, 4268.35, 4268.54, 4268.87, 4269.34, 4269.45, 4269.71, 4269.65, 4269.53, 4271.44, 4271.79, 4272.59, 4273.4, 4275.24, 4275.93, 4277.48, 4279.16, 4279.63, 4279.96, 4279.77, 4278.62, 4277.76, 4277.69, 4276.38, 4274.53, 4274.75, 4274.22, 4273.36, 4273.06, 4274.1, 4274.58, 4274.39, 4273.69, 4273.59, 4273.42, 4274.55, 4274.88, 4275.24, 4275.79, 4276.97, 4277.73, 4277.86, 4276.9, 4276.67, 4276.51, 4277.71, 4278.97, 4279.66, 4280.3, 4283.54, 4283.85, 4283.47, 4283.8, 4283.93, 4283.12, 4283.78, 4284.04, 4284.05, 4282.95, 4282.59, 4281.42, 4282.32, 4282.55, 4283.03, 4283.41, 4284.76, 4285.91, 4287.02, 4287.5, 4287.54, 4288.07, 4288.66, 4288.63, 4287.33, 4286.42, 4285.67, 4284.88, 4285.15, 4285.96, 4286.39, 4286.64, 4286.46, 4286.51, 4287.53, 4288.76, 4289.94, 4291.11, 4291.99, 4292.35, 4292.04, 4291.05, 4291.4, 4291.49, 4292.11, 4293.09, 4294.41, 4294.44, 4294.44, 4295.57, 4280.6, 4281.75, 4281.85, 4282.45, 4283.18, 4283.78, 4284.8, 4286.38, 4286.39, 4284.69, 4285.76, 4288.86, 4291.94, 4293.01, 4292.72, 4292.76, 4292.39, 4291.78, 4289.97, 4290.33, 4290.15, 4289.21, 4286.76, 4283.49, 4281.91, 4281.89, 4283.21, 4283.07, 4281.38, 4278.85, 4281.82, 4284.03, 4285.24, 4285.55, 4287.56, 4288.93, 4291.05, 4291.91, 4292.7, 4292.81, 4292.78, 4292.99, 4290.27, 4290.2, 4290.49, 4291.12, 4291.37, 4292.19, 4293.49, 4294.32, 4294.55, 4293.94, 4292.24, 4292.46, 4293.7, 4294.22, 4295.17, 4296.73, 4297.54, 4297.36, 4296.39, 4296.17, 4297.64, 4298.74, 4300.21, 4301.66, 4301.78, 4301.21, 4301.94, 4302.06, 4301.27, 4299.31, 4298.98, 4298.93, 4298.55, 4297.98, 4298.05, 4297.43, 4298.26, 4298.37, 4296.8, 4292.56, 4292.67, 4292.76, 4291.43, 4291.41, 4292.17, 4292.18, 4290.88, 4290.65, 4290.52, 4288.36, 4287.54, 4286.59, 4287.58, 4287.8, 4287.55, 4288.25, 4288.86, 4288.79, 4287.24, 4286.52, 4286.05, 4286.19, 4286.75, 4287.09, 4286.83, 4286.61, 4286.32, 4285.26, 4285.15, 4285.74, 4285.92, 4286.46, 4287.74, 4289.19, 4289.86, 4290.1, 4290.39, 4290.74, 4290.99, 4289.92, 4289.4, 4288.94, 4288.65, 4288.4, 4288.38, 4288.25, 4288.7, 4289.05, 4289.28, 4289.35, 4289, 4287.56, 4287.32, 4287.87, 4289.06, 4289.98, 4291.63, 4292.76, 4293.48, 4293.41, 4292.53, 4292.87, 4292.82, 4293.38, 4293.34, 4293.36, 4293.4, 4292.5, 4293.28, 4293.32, 4291.77, 4291.59, 4290.49, 4289.15, 4288.73, 4288.71, 4288.1, 4288.22, 4288.2, 4287.48, 4288.04, 4288.6, 4288.8, 4288.86, 4288.8, 4288.54, 4287.93, 4287.49, 4287.28, 4286.94, 4287.04, 4287.02, 4287.71, 4288, 4289.07, 4290.14, 4291.07, 4291.45, 4291.02, 4291.59, 4291.66, 4291.25, 4290.71, 4290.68, 4290.79, 4290.08, 4289.53, 4289.23, 4288.57, 4287.75, 4286.33, 4286.07, 4286.37, 4286.42, 4285.88, 4286.64, 4286.92, 4286.84, 4287.33, 4288.44, 4288.76, 4288.95, 4289.2, 4290.2, 4290.31, 4289.86, 4290.03, 4289.99, 4290.4, 4290.48, 4290.12, 4289.6, 4289.36, 4289.78, 4290.5, 4290.82, 4291.8, 4293.17, 4293.42, 4293.14, 4293.79, 4294.33, 4294.46, 4295.06, 4295.8, 4296.4, 4296.4, 4296.17, 4295.82, 4295.59, 4295.27, 4295.62, 4295.88, 4295.63, 4296.22, 4296.2, 4296.2, 4296.24, 4309.21, 4308.41, 4307.66, 4308, 4311.74, 4311.66, 4310.82, 4307.79, 4308.88, 4309.69, 4314.04, 4314.75, 4314.65, 4314.76, 4314.87, 4315.13, 4316.59, 4320.25, 4320.29, 4320.55, 4320.82, 4320.8, 4319.57, 4321.03, 4321.51, 4322.42, 4322.29, 4322.95, 4328.23, 4329.26, 4330.95, 4330.84, 4328.37, 4329.32, 4329.48, 4329.21, 4328.29, 4327.47, 4325.44, 4324.83, 4325.82, 4326.04, 4325.17, 4323.45, 4323.61, 4326.96, 4327.17, 4326.95, 4326.18, 4325.93, 4324.69, 4322.04, 4324.33, 4324.78, 4325.33, 4328.56, 4329.22, 4329.19, 4329.88, 4330.39, 4330.81, 4329.54, 4328.2, 4327.91, 4327.7, 4326.94, 4326.51, 4324.91, 4323.54, 4321.92, 4321.07, };
	float realLow2[] 	= 	{4267.11, 4267.66, 4267.5, 4267.73, 4268.98, 4269.64, 4270.54, 4271.25, 4270.75, 4270.47, 4270.57, 4270.82, 4271.05, 4271.24, 4271.17, 4270.68, 4270.63, 4270.33, 4270.62, 4271.11, 4271.45, 4272.46, 4273.05, 4274.15, 4274.15, 4274.65, 4257.59, 4257.8, 4258.38, 4254.92, 4254.51, 4256.35, 4257.19, 4256.56, 4254.88, 4251.33, 4247.33, 4247.3, 4253.08, 4254.66, 4255.15, 4255.41, 4257.05, 4258.67, 4256.8, 4256.23, 4254.94, 4254.7, 4254.62, 4254.74, 4256.6, 4260.54, 4260.68, 4261.68, 4261.62, 4261.26, 4256.18, 4254.44, 4254.34, 4256.18, 4256.03, 4256.13, 4257.58, 4258.34, 4256.36, 4256.08, 4254.72, 4254.38, 4254.27, 4254.73, 4255.09, 4254.86, 4254.56, 4254.57, 4256.49, 4259.5, 4259.94, 4260.42, 4260.62, 4261.7, 4264.6, 4264.1, 4263.85, 4262.62, 4262.36, 4262.07, 4259.67, 4259.69, 4259.72, 4259.32, 4259.26, 4260.62, 4260.45, 4261.18, 4259.71, 4258.41, 4256.82, 4255.89, 4255.68, 4255.34, 4254.51, 4254.42, 4254.47, 4254.41, 4254.33, 4255.63, 4255.37, 4255.2, 4255.12, 4255.41, 4255.27, 4255.26, 4255.33, 4254.97, 4254.07, 4252.67, 4249.77, 4248.64, 4249.4, 4249.57, 4249.48, 4249.31, 4249.83, 4251.56, 4251.7, 4251.75, 4251.82, 4253.91, 4254.29, 4254.34, 4254.48, 4254.19, 4254.29, 4255.28, 4256.83, 4260.36, 4260.86, 4260.56, 4260.1, 4260, 4261.3, 4262.33, 4264.87, 4265.59, 4267.13, 4267.73, 4268.53, 4268.38, 4267.79, 4267.19, 4267.19, 4267.91, 4267.08, 4267.01, 4267.19, 4269.04, 4270.27, 4272.15, 4273.19, 4272.04, 4271.48, 4271.49, 4271.05, 4270.87, 4271.32, 4270.99, 4268.85, 4268.35, 4267.92, 4268.04, 4268.15, 4268.57, 4268.83, 4269.03, 4269.04, 4268.65, 4268.84, 4271.18, 4271.54, 4272.6, 4273.3, 4275.21, 4276.2, 4277.85, 4279.04, 4279.42, 4278.48, 4277.57, 4277.31, 4276.35, 4274.22, 4274.14, 4273.93, 4273.02, 4272.53, 4272.68, 4272.97, 4274.07, 4273.03, 4273.23, 4273.11, 4272.97, 4273.16, 4274.34, 4274.82, 4275.21, 4275.56, 4277.01, 4276.87, 4276.41, 4276.07, 4276.12, 4276.26, 4277.67, 4278.88, 4279.6, 4280.38, 4282.96, 4282.95, 4283.46, 4282.85, 4282.79, 4282.83, 4283.62, 4282.98, 4282.53, 4281.29, 4280.92, 4281.06, 4282.16, 4282.33, 4282.78, 4283.19, 4284.85, 4285.94, 4286.92, 4287.16, 4287.08, 4287.94, 4287.29, 4286.25, 4285.48, 4284.83, 4284.39, 4284.43, 4284.91, 4285.78, 4285.95, 4286, 4285.9, 4286.17, 4287.41, 4288.75, 4289.74, 4291.12, 4291.82, 4290.71, 4290.68, 4291.1, 4290.95, 4291.23, 4291.92, 4293, 4294.38, 4294.44, 4295.22, 4276.54, 4277.94, 4280.6, 4280.74, 4280.92, 4283.13, 4283.68, 4284.54, 4284.71, 4284.22, 4284.71, 4286.1, 4289.25, 4292.45, 4292.24, 4291.75, 4292.05, 4289.93, 4288.83, 4289.24, 4288.91, 4287.06, 4283.69, 4282.09, 4279.81, 4279.75, 4281.84, 4281.48, 4278.97, 4278.41, 4278.77, 4281.75, 4283.91, 4284.78, 4285.61, 4287.71, 4288.88, 4291.09, 4291.89, 4291.79, 4291.64, 4289.97, 4289.52, 4289.88, 4289.96, 4290.49, 4290.78, 4291.42, 4292.1, 4293.55, 4293.6, 4291.47, 4291.5, 4291.78, 4292.44, 4293.63, 4294.27, 4295.15, 4296.94, 4295.27, 4295.12, 4295.25, 4296.18, 4297.76, 4298.68, 4300.42, 4300.36, 4300.49, 4301.08, 4301.38, 4299.28, 4298.39, 4298.47, 4298.27, 4297.15, 4297.54, 4296.36, 4296.29, 4297.48, 4297.06, 4292.66, 4292.13, 4292.07, 4291.1, 4290.78, 4290.78, 4291.15, 4291.38, 4290.22, 4290.16, 4288.33, 4287.34, 4286.05, 4285.74, 4286.33, 4286.75, 4286.98, 4287.24, 4288.08, 4286.28, 4286.51, 4286.15, 4285.64, 4285.51, 4285.87, 4286.58, 4286.38, 4286.07, 4284.62, 4284.66, 4284.27, 4285.14, 4285.41, 4285.52, 4286.31, 4287.85, 4289.21, 4289.57, 4289.91, 4289.94, 4289.75, 4288.97, 4288.75, 4288.3, 4288.2, 4287.94, 4287.54, 4287.7, 4288.22, 4288.57, 4288.59, 4288.5, 4287.37, 4286.74, 4286.79, 4287.21, 4287.74, 4288.76, 4290.02, 4291.3, 4292.62, 4292.39, 4292.11, 4292.22, 4292.24, 4292.54, 4292.75, 4292.87, 4292.29, 4292, 4291.95, 4291.51, 4291.23, 4290.17, 4289.22, 4288.06, 4288, 4287.63, 4287.41, 4287.88, 4286.9, 4286.79, 4287.44, 4287.98, 4288.38, 4288.17, 4288.26, 4287.38, 4287.26, 4286.97, 4286.44, 4286.44, 4286.66, 4286.49, 4286.88, 4287.53, 4287.86, 4289.11, 4290.12, 4290.74, 4290.65, 4290.95, 4291.08, 4290.39, 4290.11, 4290.19, 4289.89, 4289.36, 4289.03, 4288.31, 4287.62, 4285.94, 4285.73, 4285.62, 4285.65, 4285.64, 4285.29, 4285.66, 4286.36, 4286.38, 4286.41, 4287.31, 4288.27, 4288.56, 4288.61, 4289.35, 4289.44, 4289.58, 4289.52, 4289.42, 4289.76, 4289.82, 4289.19, 4289.17, 4288.75, 4288.86, 4289.43, 4290.12, 4290.71, 4291.73, 4292.8, 4292.69, 4292.84, 4293.56, 4294.05, 4294.26, 4295.1, 4295.75, 4296.02, 4295.76, 4295.28, 4294.92, 4294.7, 4294.75, 4295.29, 4295.31, 4295.48, 4296.08, 4296.2, 4296.21, 4307.56, 4307.3, 4306.35, 4306.58, 4308.03, 4310.89, 4308.26, 4305.8, 4305.64, 4308.93, 4309.38, 4313.9, 4313.85, 4314.26, 4311.3, 4311.3, 4315.04, 4316.38, 4319.82, 4320.02, 4320.19, 4319.23, 4318.7, 4319.43, 4320.82, 4321.51, 4321.54, 4321.45, 4322.99, 4328.02, 4329.1, 4328.19, 4327.34, 4327.77, 4328.7, 4328.18, 4327.52, 4325.46, 4323.66, 4323.37, 4324.6, 4325.02, 4323.6, 4323.07, 4323.04, 4323.53, 4326.88, 4325.88, 4325.59, 4324.7, 4322.23, 4321.61, 4321.72, 4324.19, 4324.43, 4325.21, 4328.79, 4328.9, 4328.89, 4329.77, 4329.54, 4327.31, 4327.12, 4327.44, 4326.54, 4326.06, 4325.16, 4323.53, 4321.94, 4320.51, 4320.49,};
	
	testChanLun("TEST 11, real data test", realHigh2, realLow2, sizeof(realHigh2)/sizeof(realHigh2[0]));

	
	

	return 1;
}




