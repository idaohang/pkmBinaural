/*
 *  pkmBinauralizer.h
 *  Copyright Parag K Mital 2011
 *  http://pkmital.com
 
 
 
 LICENCE
 
 pkmSonicGraffiti "The Software" © Parag K Mital, parag@pkmital.com
 
 The Software is and remains the property of Parag K Mital
 ("pkmital") The Licensee will ensure that the Copyright Notice set
 out above appears prominently wherever the Software is used.
 
 The Software is distributed under this Licence: 
 
 - on a non-exclusive basis, 
 
 - solely for non-commercial use in the hope that it will be useful, 
 
 - "AS-IS" and in order for the benefit of its educational and research
 purposes, pkmital makes clear that no condition is made or to be
 implied, nor is any representation or warranty given or to be
 implied, as to (i) the quality, accuracy or reliability of the
 Software; (ii) the suitability of the Software for any particular
 use or for use under any specific conditions; and (iii) whether use
 of the Software will infringe third-party rights.
 
 pkmital disclaims: 
 
 - all responsibility for the use which is made of the Software; and
 
 - any liability for the outcomes arising from using the Software.
 
 The Licensee may make public, results or data obtained from, dependent
 on or arising out of the use of the Software provided that any such
 publication includes a prominent statement identifying the Software as
 the source of the results or the data, including the Copyright Notice
 and stating that the Software has been made available for use by the
 Licensee under licence from pkmital and the Licensee provides a copy of
 any such publication to pkmital.
 
 The Licensee agrees to indemnify pkmital and hold them
 harmless from and against any and all claims, damages and liabilities
 asserted by third parties (including claims for negligence) which
 arise directly or indirectly from the use of the Software or any
 derivative of it or the sale of any products based on the
 Software. The Licensee undertakes to make no liability claim against
 any employee, student, agent or appointee of pkmital, in connection 
 with this Licence or the Software.
 
 
 No part of the Software may be reproduced, modified, transmitted or
 transferred in any form or by any means, electronic or mechanical,
 without the express permission of pkmital. pkmital's permission is not
 required if the said reproduction, modification, transmission or
 transference is done without financial return, the conditions of this
 Licence are imposed upon the receiver of the product, and all original
 and amended source code is included in any transmitted product. You
 may be held legally responsible for any copyright infringement that is
 caused or encouraged by your failure to abide by these terms and
 conditions.
 
 You are not permitted under this Licence to use this Software
 commercially. Use for which any financial return is received shall be
 defined as commercial use, and includes (1) integration of all or part
 of the source code or the Software into a product for sale or license
 by or on behalf of Licensee to third parties or (2) use of the
 Software or any derivative of it for research with the final aim of
 developing software products for sale or license to a third party or
 (3) use of the Software or any derivative of it for research with the
 final aim of developing non-software products for sale or license to a
 third party, or (4) use of the Software to provide any service to an
 external organisation for which payment is received. If you are
 interested in using the Software commercially, please contact pkmital to
 negotiate a licence. Contact details are: parag@pkmital.com
 
 
 *
 *
 */

#ifndef pkmBinauralizer_
#define pkmBinauralizer_

#include "pkmIRCAM_HRTF_DATABASE.h"			// HRTFs
#include "ann/ANN.h"						// kd-tree
#include <Accelerate/Accelerate.h>

//#pragma GCC visibility push(default)

class pkmBinauralizer
{
	
public:
	pkmBinauralizer();
	~pkmBinauralizer();
	
	// setup binauralizer
	void initialize(int window_size = 512);
	
	// Get the hrtf's closest to a 3D pt
	// 
	// input takes a floating pt x,y,z (in meters)
	//
	// results are in: 
	//	* nnIdx (indices of 3 nearest filters)
	//	* dists (float pt distances of these filters from pt x,y,z)
	//
	void updateNearestFilters(float x, float y, float z);
	
	// call at audio-rate every 512 samples
	// uses FIR convolution
	// encode a mono source signal to 2-channel using the HRTFs calculated from
	// updateNearestFilters
	void binauralize(float *input, float *lOutput, float *rOutput);
	
	void linearConvolution(float *X, size_t lenx, float *Y, size_t leny, float *Z, size_t &lenz);
	
private:
	
	// private initializations
	void initializeFilters();
	void initializeKDTree();
	
	// HRTFs
	ircam_hrtf_filter_set			m_hrtfs;		// frequency domain filters
	
	// For kNN
	ANNkd_tree						*kdTree;		// distances to nearest HRTFs
	ANNidxArray						nnIdx;			// near neighbor indices
	ANNdistArray					dists;			// near neighbor distances
	int								k;				// number of nearest neighbors
	int								dim;			// dimension of each point
	int								pts;			// number of points
	ANNpointArray					positions;		// positions of each filter
	
	// position
	double							prev_x, prev_y, prev_z;

	// For filtering
	size_t							filterLength, paddedFilterLength, convolutionLength, inputLength;
	float							*windowedData,
									*weightedLData1,
									*weightedRData1,
									*weightedLData2,
									*weightedRData2,
									*weightedLData3,
									*weightedRData3,
									*weightedLDataSummed,
									*weightedRDataSummed,
									*previousOutputLData,
									*previousOutputRData,
									*currentOutputLData,
									*currentOutputRData;
	
	FFTSetup						fftSetup;
	size_t							fftSize, 
                                    fftSizeOver2, 
                                    log2n, 
                                    log2nhalf, 
                                    bufferSize, 
                                    paddedBufferSize;
	COMPLEX_SPLIT					signal_fft, 
                                    l_ir_fft, 
                                    r_ir_fft, 
                                    l_filtered_signal_fft, 
                                    r_filtered_signal_fft;
    
	float							*fftWindow, 
                                    *windowedSignal, 
                                    *left_result, 
                                    *right_result, 
                                    *paddedSignal,
									*paddedFilter_l, 
                                    *paddedFilter_r, 
                                    *prevLOverlap, 
                                    *prevROverlap;
	
	bool							bInitialized;

};

//#pragma GCC visibility pop
#endif
