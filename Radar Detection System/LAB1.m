%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Uint8 Datatransfer from FreeSoc2 to Matlab
% Version 1.1, Bannwarth, 27.05.2020
%
% Behaviour: 
% 1. Sends 's' over the serial port.
% 2. Waits until there are 256 bytes in the buffer.
% 3. If there are 256 bytes in the buffer, it reads them, calculates the FFT 
%    of the data, and displays the received data and the absolute value of the FFT.
%
% Using:
% 1. Connect FreeSoc2 to USB (i.e., power up).
% 2. Check the correct serial port settings.
% 3. Run this script.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Close all figures, clear workspace and command window
close all;
clear all;
clc;


N = 1024;      % Sample Size
NG = 1;        % Guard Cells
NR = 5;        % Reference Cells
PFA = 10^-6;  % Probability of False Alarm

% Setup and open the serial communication in Matlab
priorPorts = instrfind; % Find any existing serial port objects
delete(priorPorts); % Delete any existing serial port objects

% Create a serial port object
PSoC = serial('COM7', 'BaudRate', 115200, 'InputBufferSize', 14000); % Specify COM port and settings
fopen(PSoC); % Open the serial port

f1 = figure;
f2 = figure;
f3 = figure;
f4 = figure;

count = 1;
flg_data_avai = 0;
fwrite(PSoC,'s','uchar') % means send, I am ready to receive
while(flg_data_avai == 0)
   fprintf("Bytes available: %d\n", PSoC.BytesAvailable);

 if PSoC.BytesAvailable == 10240
 fwrite(PSoC,'o','uchar') % means I received all expected data
 rx_data_adc = fread(PSoC,1024,'uint16');
 rx_data_fft = fread(PSoC,2048,'int32');

% FFT manupulations for FFT on PSoC
fft_real = rx_data_fft(1:2:end);
fft_imag = rx_data_fft(2:2:end);
fft_mag = sqrt(fft_real.^2 + fft_imag.^2);


% Plotting the received data : FFT on MATLAB
figure(f1)
subplot(2,1,1)
plot([0:(length(rx_data_adc)-1)],rx_data_adc(1:(length(rx_data_adc))));
title(['Received Time Domain Data No.:',num2str(count)]);
subplot(2,1,2)
plot([0:1023],20*log10((1/length(rx_data_adc))*abs(fft(rx_data_adc))));
title('FFT -  MATLAB');
hold on;
plot([0:1023],20*log10((fft_mag)));
title('FFT - PSoC');
legend('FFT -  MATLAB','FFT - PSoC');
hold off;



% CA CFAR Implementation
fft2 = fft_mag.^2;

%% CFAR MATLAB ALGORITHM

% Apply CFAR detection using phased.CFARDetector
cfar = phased.CFARDetector('NumTrainingCells', 2*NR, 'NumGuardCells', 2*NG, 'ProbabilityFalseAlarm', PFA, 'ThresholdFactor', 'Auto');
cfar.ThresholdOutputPort = true;

[x_detected, th] = cfar(fft2, 1:length(fft2));

% Plot the CFAR detection results
figure(f3)
plot(fft2, 'b');
hold on
plot([0:1023],th, 'r--', 'LineWidth', 1.5); % Plot threshold
plot([0:1023],x_detected * max(xnoise_fft2), 'g*'); % Plot detected points scaled for visualization
legend('Noisy Signal FFT Power', 'Threshold', 'Detected Points');
title('CFAR Detection using MATLAB Library');
xlabel('Sample Number');
ylabel('Amplitude');

threshold_m = zeros(N,1);
CFAR_output_m = zeros(N,1);

% Loop through the array indices
for i = NR + NG + 1:N - NR - NG
   
    noise_level_m = 0;

    % Sum over the left reference cells
    for j = i - NR - NG:i - NG - 1      
            noise_level_m = noise_level_m +  fft2(j); 
    end
    % Sum over the right reference cells
     for j = i + NG + 1:i + NG + NR
            noise_level_m = noise_level_m + fft2(j);    
     end
     
    noise_level_m =  noise_level_m / (5 * NR); % Average noise level

    alpha = NR*(PFA^(-1/NR) - 1);

    % Compute Threshold
    threshold_m(i) =  ( alpha * noise_level_m );
    
    % Apply Threshold to the CUT (Cell Under Test)
    if fft2(i) > threshold_m(i)
        CFAR_output_m(i) = fft2(i);
    end
     
end


    % Plot the CFAR detection results
    figure(f4)
    plot([0:1023], fft2, 'b');
    hold on
    plot([0:1023], threshold_m, 'r--', 'LineWidth', 1.5); % Plot threshold
    plot([0:1023], CFAR_output_m, 'g*');                  % Plot detected points
    legend('Noisy Signal FFT Power', 'Threshold', 'Detected Points');
    title('CFAR Detection without Library');
    xlabel('Sample Number');
    ylabel('Amplitude');
    
   fprintf(" Transfer %i DONE \n",count);

  fwrite(PSoC,'s','uchar') % means send, I am ready to receive
      
 % Save the received data
 count=count+1;
 end
 if count == 11
 break;

end
end
fclose(PSoC);
fprintf(" Scipt End \n");