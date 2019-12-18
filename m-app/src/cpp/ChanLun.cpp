
#include <stdlib.h>
#include <fstream>
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

int stopIndexes[] = {143, 207, 214, 225};

void debugStopIndex(int index) {
	if (!testMode) return;
	for (int i = 0; i < (sizeof(stopIndexes) / sizeof(stopIndexes[0])); ++i) {
		if (index == stopIndexes[i]) {
			cout << "stop here" << endl;
			return;
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
	return (ml.high - ml.low) < 0.001;
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
	if (curIndex >= DataLen) return 0;
	if (curIndex == 0) {
		return (mergedLines[0].high - mergedLines[1].high) > 0 ? 1 : -1;
	}
	if (curIndex == DataLen - 1) {
		return (mergedLines[curIndex].high - mergedLines[curIndex - 1].high) > 0 ? 1 : -1;
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

	// If end of merged lines, return
	if (endIndex == DataLen - 1) {
		return direction;
	}
	
	// TODO: 缺口
	// Only when it is opposite trend
	//   1) If direction is DOWN, and the first ML's low < previous Pen's low.
	//   2) If direction is UP, and the first ML's high > previous Pen's high.
	MergedLine startML = mergedLines[startIndex];
	MergedLine endML = mergedLines[endIndex];
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
		// Back included judgement.
		if (backIncldued) {
			MergedLine endRightML = mergedLines[endIndex + 1];
			if (direction > 0) {
				//If opposite trend may exist
				if (endRightML.high < endML.low && endRightML.low < startML.low) {
					float previousPenVal = abs(previousPen.high - previousPen.low);
					// Opposite trend exist
					if ((endML.high - endRightML.high) > previousPenVal * oppositeTrendFactor) {
						return direction;
					}
				}

				// If UP, and the endML's right ML'low < start.low, included.
				if (endRightML.low < startML.low) {
					return 0;
				}
			} else {
				//If opposite trend may exist
				if (endRightML.low > endML.high && endRightML.high > startML.high) {
					float previousPenVal = abs(previousPen.high - previousPen.low);
					// Opposite trend exist
					if ((endRightML.low - endML.low) > previousPenVal * oppositeTrendFactor) {
						return direction;
					}
				}

				// If DOWN, and the endML's right ML'high > start.high, included.
				if (endRightML.high > startML.high) {
					return 0;
				}
			}
			
		}
		
		// Forward included judgement.
		if (forwardIncluded) {
			MergedLine startLeftML = mergedLines[startIndex - 1];
			if (direction > 0) {
				// if opposite trend may exist
				if (startLeftML.low > startML.high && startLeftML.high > endML.high) {
					float previousPenVal = abs(endML.high - startML.low);
					// Opposite trend exists
					if ((startLeftML.low - startML.low) > previousPenVal * oppositeTrendFactor) {
						return direction;
					}
				}

				// If UP, and the startML's left ML'high > endML.high, included. 
				if (startLeftML.high > endML.high) {
					return 0;
				}
			} else {
				// if opposite trend may exist
				if (startLeftML.high < startML.low && startLeftML.low < endML.low) {
					float previousPenVal = abs(startML.high - endML.low);
					// Opposite trend exists
					if ((startML.high - startLeftML.high) > previousPenVal * oppositeTrendFactor) {
						return direction;
					}
				}

				// If DOWN, and the startML's left ML'low < endML.low, included. 
				if (startLeftML.low < endML.low) {
					return 0;
				}
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
				else if (rootPen.low > res.new_low && rootPen.high < res.new_high) {
					return {false, highest_index, lowest_index, highest, lowest};
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
				else if (rootPen.low > res.new_low && rootPen.high < res.new_high) {
					return {false, highest_index, lowest_index, highest, lowest};
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

void DownloadData(int DataLen, float* pfOUT, float* highInput, float* lowInput, float* closeInput) {
	ofstream localFile;
	localFile.open("test111.log");
	localFile << "High:" << endl;
	for (int i = 0; i < DataLen; ++i) {
		localFile << highInput[i] << ", ";
	}
	localFile << "\n\nlow:" << endl;
	for (int i = 0; i < DataLen; ++i) {
		localFile << lowInput[i] << ", ";
	}
	
	localFile << "\n\nwrite test data end\n";
	localFile.close();
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
	float realHigh2[] 	= 	{695.3, 696.13, 696.04, 699.06, 699.37, 699.33, 698.66, 697.76, 697.81, 698.34, 698.45, 698.52, 699.77, 699.97, 700.3, 700.25, 702.26, 701.89, 700.71, 700.33, 700.34, 699.04, 698.11, 698.47, 698.46, 698.05, 697.48, 696.41, 696.5, 696.45, 696.08, 696.51, 696.56, 696.97, 697.52, 697.35, 697.48, 697.36, 697.59, 697.88, 698.02, 697.52, 698.03, 698.12, 697.85, 697.23, 697.31, 697.09, 698.12, 698.03, 698.02, 698.28, 697.88, 698.66, 698.71, 699.18, 699.44, 699.71, 700.78, 701.25, 704.68, 702.27, 700.03, 700.36, 700.25, 699.87, 701.15, 700.88, 700.02, 700.37, 701.97, 702.32, 703.42, 703.78, 703.09, 702.67, 702.88, 702.27, 702.06, 701.95, 701.2, 700.77, 700.89, 700.54, 700.37, 700.23, 700.22, 700.31, 700.2, 700.55, 701.14, 701.31, 700.98, 701.41, 700.97, 700.51, 700.17, 699.46, 699.69, 700.21, 700.31, 700.38, 701.56, 701.61, 700.93, 700.9, 700.61, 700.4, 699.03, 698.37, 698.16, 697.74, 697.58, 696.47, 696.3, 696.79, 697.38, 697.06, 696.96, 696.84, 697.37, 697.35, 696.57, 696.07, 696.15, 695.79, 695.33, 694.72, 695.16, 695.35, 695.29, 695.84, 696.03, 695.87, 696.3, 697.05, 697.11, 697.06, 697.11, 696.75, 696.83, 698.66, 698.66, 698.26, 698.37, 698.49, 700.01, 700.08, 699.42, 698.8, 699.41, 699.51, 699.22, 699.33, 699.31, 698.81, 700.71, 700.04, 700.03, 699.26, 698.31, 697.48, 697.84, 697.88, 697.29, 697.39, 697.4, 698.8, 703.26, 706.79, 704.45, 703.18, 703.13, 702.26, 701.62, 700.95, 701.42, 701.58, 701.77, 702.08, 702.05, 701.75, 701.38, 701.4, 701.24, 700.56, 700.47, 700.29, 701.15, 701.25, 701.08, 701.25, 702.07, 701.78, 701.16, 700.53, 700.31, 700.44, 700.08, 699.82, 699.63, 699.71, 699.75, 699.71, 699.45, 698.19, 697.69, 696.99, 697.32, 697.74, 698.28, 697.94, 697.82, 697.31, 696.93, 697.63, 698.67, 698.87, 699.68, 699.32, 698.92, 698.55, 699.34, 699.31, 698.69, 698.52, 698.01, 697.83, 697.98, 698.07, 698.17, 698.28, 697.9, 697.7, 697.32, 697.01, 697.04, 696.5, 695.89, 696.22, 696.14, 696.43, 696.27, 696.58, 696.56, 698.35, 698.57, 696.88, 696.2, 695.66, 695.85, 696.02, 707.97, 707.62, 708.87, 713.41, 713.97, 710.94, 711.43, 710.71, 709.72, 710.24, 711.4, 712.03, 710.17, 710.19, 708.78, 708.22, 708, 706.57, 706.92, 707.38, 707.65, 709.3, 712.76, 711.38, 712.82, 713.01, 714.68, 716.06, 714.61, 717.1, 720.92, 720.78, 725.05, 727.75, 725.36, 725.01, 721.41, 721.71, 722.78, 722.34, 722.83, 723.11, 723.13, 722.04, 723.04, 723.07, 723.47, 722.54, 723.24, 719.69, 722.45, 725.29, 726.59, 723.34, 725.94, 725.5, 724.52, 723.73, 722.09, 722.11, 722, 721.56, 720.26, 720.33, 721.67, 722.71, 723.22, 723.53, 723.33, 722.95, 725.29, 725.52, 725.73, 725.74, 729.41, 732.55, 733.12, 731.15, 735.16, 736.08, 738.23, 737.39, 735.61, 735.6, 736.62, 736.95, 736.85, 734.73, 732.77, 733.05, 733.06, 733.75, 733.91, 735.11, 735.15, 734.08, 735.96, 736.78, 735.03, 733.12, 731.84, 733.04, 736.68, 737.29, 735.93, 737.41, 734.43, 734.44, 735.04, 735.3, 736.12, 738.3, 740.75, 741.21, 740.45, 747.97, 752.97, 754.19, 757.1, 765.71, 775.52, 776.32, 775.46, 772.64, 771.96, 772.3, 773.37, 773.59, 770.31, 766.09, 763.43, 763, 762.91, 762.26, 759.55, 759.39, 760.99, 761.49, 760.57, 759.69, 757.9, 756.7, 755.3, 754.23, 760.26, 763.87, 757.33, 757.04, 761.36, 763.91, 762.53, 766.14, 767.64, 765.08, 763.14, 761.66, 759.11, 758.99, 764.46, 766.45, 765.18, 763.25, 761.42, 760.42, 759.6, 759.36, 759.12, 759.17,};
	float realLow2[] 	= 	{694.75, 695.02, 695.56, 695.74, 698.41, 698.39, 697.4, 697.26, 697.25, 697.32, 698.12, 698.17, 697.77, 698.17, 699.6, 699.05, 700.09, 700.36, 700.09, 699.58, 698.98, 697.68, 697.57, 697.97, 697.63, 697.07, 696.21, 695.96, 696.04, 695.49, 695.32, 696.05, 696.1, 696.11, 696.93, 696.84, 697.05, 696.95, 696.95, 697.5, 697.37, 697.12, 697.32, 697.65, 696.8, 696.74, 696.88, 696.36, 696.68, 697.53, 697.54, 697.63, 697.31, 697.34, 698.18, 698.33, 698.62, 699.12, 699.4, 700.52, 702.16, 699.6, 698.35, 699.8, 699.56, 699.28, 699.66, 700.07, 699.27, 699.71, 700.2, 701.86, 702.02, 702.88, 702.15, 702.06, 702.01, 701.66, 701.53, 701.14, 700.29, 700.39, 700.18, 700.09, 699.87, 699.82, 699.75, 699.84, 699.62, 699.72, 700.29, 700.85, 700.53, 700.74, 700.25, 699.79, 699.01, 698.95, 699.2, 699.36, 699.81, 699.97, 700.07, 700.35, 700.15, 700.22, 699.91, 700.01, 697.43, 697.52, 697.25, 697.04, 696.12, 695.65, 695.53, 695.87, 696.31, 696.51, 696.18, 696.41, 696.5, 696.17, 695.66, 695.45, 695.48, 695.07, 694.2, 694.11, 694.26, 694.68, 694.87, 694.8, 695.1, 695.18, 695.41, 696.26, 696.53, 696.43, 696.42, 696.01, 696.4, 696.43, 698.22, 697.43, 697.48, 698.05, 698.37, 699.19, 698.67, 698.37, 698.46, 698.96, 698.51, 698.55, 698.51, 698.46, 699.19, 699.34, 698.72, 698.12, 697.09, 696.57, 696.58, 696.93, 696.67, 696.66, 696.86, 697.31, 698.88, 702.81, 702.55, 702.57, 701.81, 701.42, 700.37, 700.41, 700.43, 701.19, 701.3, 701.44, 701.32, 701.21, 700.72, 700.76, 700.24, 699.84, 699.78, 699.92, 699.85, 700.59, 700.61, 700.79, 701.04, 700.96, 700.5, 699.86, 699.84, 699.83, 699.28, 699.38, 699.15, 699.3, 699.13, 699.18, 697.53, 697.22, 696.51, 696.2, 696.47, 696.3, 697.58, 697.51, 697.03, 696.01, 696.16, 696.36, 696.89, 698.04, 698.63, 698.68, 698.21, 697.99, 698.3, 698.41, 698.11, 697.72, 697.36, 697.19, 697.09, 697.09, 697.42, 697.56, 697.36, 696.8, 696.71, 696.27, 696.28, 695.66, 695.41, 695.45, 695.37, 695.72, 695.6, 695.81, 696.13, 696.06, 696.89, 695.97, 695.37, 695.03, 695.17, 695.48, 705.82, 705.26, 705.94, 708.79, 710.89, 709.16, 709.66, 709.6, 707.89, 707.99, 709.29, 709.57, 709.42, 708.49, 707.57, 707.68, 705.96, 705.54, 706.22, 706.39, 706.47, 707.21, 709.11, 710.63, 711, 711, 710.97, 713.61, 713.91, 714.2, 717.1, 719.19, 720.81, 724.55, 723.59, 721.27, 720.73, 720.9, 721.47, 721.63, 721.65, 722.17, 721.49, 721.37, 721.85, 722.44, 722.43, 721.59, 717.98, 717.42, 719.77, 721.61, 723.06, 722.06, 722.19, 724.36, 723.61, 721.78, 720.9, 721.18, 720.83, 719.67, 719.63, 719.31, 720.19, 721.11, 722.57, 722.7, 722.48, 722.37, 722.89, 724.57, 724.99, 724.29, 724.45, 729.04, 730.53, 730.23, 730.77, 734.79, 736.04, 734.34, 734.62, 734.47, 734.77, 736.4, 734.38, 732.59, 731.71, 732.63, 732.55, 732.55, 733.15, 733.4, 733.94, 733.7, 731.82, 734.14, 732.75, 730.78, 730.83, 731.18, 732.87, 735.44, 734.42, 734.36, 733.5, 733.59, 734, 734.47, 734.82, 736, 738.12, 740.43, 738.31, 739.77, 747.29, 750.68, 750.85, 757.18, 765.76, 771.34, 771.4, 769.94, 770.22, 770.27, 771.12, 770.33, 765.13, 763.68, 760.73, 762.28, 761.97, 759.65, 756.94, 758.6, 758.37, 760.29, 759.41, 757.85, 756.36, 755.26, 753.92, 753.68, 751.57, 757.21, 753.46, 752.76, 757.02, 761.28, 759.89, 761.6, 764.81, 762.08, 761.52, 759.23, 756.21, 756.37, 758.98, 762.65, 762.94, 760.09, 759, 758.95, 758.98, 758.2, 758.32, 758.52,};
	
	testChanLun("TEST 11, real data test", realHigh2, realLow2, sizeof(realHigh2)/sizeof(realHigh2[0]));

	
	

	return 1;
}




