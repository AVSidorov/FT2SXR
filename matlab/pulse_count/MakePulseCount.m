function [times, counts] = MakePulseCount(path, channel, AverageTime,MinFront,MinAmpl,StartInd)

if nargin<2; channel='00'; end;
if nargin<3; AverageTime=100; end; % us, time interval for counting pulse number
if nargin<4; MinFront=10; end; 
if nargin<5; MinAmpl=500; end; 
if nargin<6; StartInd=3e6;  end;

raw_data = h5read(path, ['/SXR/ADC/channel' channel]);
[~, rate] = CountExpPulses(raw_data, AverageTime, MinFront, MinAmpl, StartInd, 0);
times = rate(:,1);
counts = rate(:,2);

try
    h5create(path, ['/SXR/processed_data/count_rate/channel' channel '/times'] , (length(times)), Chunksize=(length(times)), Deflate=5);
    h5create(path, ['/SXR/processed_data/count_rate/channel' channel '/counts'] , (length(counts)), Chunksize=(length(counts)), Deflate=5);
    h5create(path, ['/SXR/processed_data/count_rate/channel' channel '/AverageTime'] , (1), Chunksize=(1), Deflate=5);
    h5create(path, ['/SXR/processed_data/count_rate/channel' channel '/MinFront'] , (1), Chunksize=(1), Deflate=5);
    h5create(path, ['/SXR/processed_data/count_rate/channel' channel '/MinAmpl'] , (1), Chunksize=(1), Deflate=5);
    h5create(path, ['/SXR/processed_data/count_rate/channel' channel '/StartInd'] , (1), Chunksize=(1), Deflate=5);
catch
    ;
end
    

h5write(path, ['/SXR/processed_data/count_rate/channel' channel '/times'], times);
h5write(path, ['/SXR/processed_data/count_rate/channel' channel '/counts'], counts);
h5write(path, ['/SXR/processed_data/count_rate/channel' channel '/AverageTime'], AverageTime);
h5write(path, ['/SXR/processed_data/count_rate/channel' channel '/MinFront'], MinFront);
h5write(path, ['/SXR/processed_data/count_rate/channel' channel '/MinAmpl'], MinAmpl);
h5write(path, ['/SXR/processed_data/count_rate/channel' channel '/StartInd'], StartInd);

%disp('done')
%for i = 1:length(counts)
%    disp([times(i)  counts(i)]);
%end

end
