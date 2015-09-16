/**************************************
* Module: ADC_CTRL
* Date:2014-01-05  ${time}
* Author: Mike     
*
* Description: modified version of a DE0 demo file
***************************************/
module ADC_CTRL (   
                    iRST, //key 0
                    iCLK, //SPI Clock
                    iCLK_n, // SPI clock n
                    iGO, //key 1
                    iCH, //3 bit channel select
                    oLED, //output data
                    
                    oDIN, //ADC_SADDR
                    oCS_n,//ADC_CS_N
                    oSCLK,//ADC_SCLK
                    iDOUT //ADC_SDAT
                );
                    
input               iRST;
input               iCLK;
input               iCLK_n;
input               iGO;
input   [2:0]       iCH;
output  [7:0]       oLED;

output              oDIN;
output              oCS_n;
output              oSCLK;
input               iDOUT;

reg                 data;
reg                 go_en;
wire    [2:0]       ch_sel;
reg                 sclk;
reg     [3:0]       cont;
reg     [3:0]       m_cont;
reg     [11:0]      adc_data;
reg     [7:0]       led;

assign  oCS_n       =   ~go_en;
assign  oSCLK       =   (go_en)? iCLK:1;
assign  oDIN        =   data;
assign  ch_sel      =   iCH;
assign  oLED        =   led;

always@(posedge iGO or negedge iRST)//on key press event
begin
    if(!iRST) //if reset key pressed do not enable ADC
        go_en   <=  0;
    else
    begin
        if(iGO) //if go key pressed enable ADC
            go_en   <=  1;
    end
end

always@(posedge iCLK or negedge go_en) //look for positive iCLK edge or negative edge go enable
begin
    if(!go_en) // if ADC not enabled 
        cont    <=  0; //set cont to zero
    else			//else if ADC enabled
    begin		//increment cont every iCLK
        if(iCLK)
            cont    <=  cont + 1;
    end
end

always@(posedge iCLK_n) //every positive iCLK_n edge
begin				
    if(iCLK_n)				//if positive transfer cont to m_cont
        m_cont  <=  cont;
end

always@(posedge iCLK_n or negedge go_en) // ... or negative edge of ADC go_enable
begin
    if(!go_en)				//if ADC off then set data = 0
        data    <=  0;
    else					//else if iCLK_n is positive 
    begin					//use 3 cycles to transfer dipswitch position to ADC oDIN
        if(iCLK_n)
        begin
            if (cont == 2)
                data    <=  iCH[2];
            else if (cont == 3)
                data    <=  iCH[1];
            else if (cont == 4)
                data    <=  iCH[0];
            else
                data    <=  0;
        end
    end
end

always@(posedge iCLK or negedge go_en)// ... or negative edge of ADC go_enable
begin
    if(!go_en)		//if ADC not enabled set data to 0 and led output low
    begin
        adc_data    <=  0;
        led         <=  8'h00;
    end
    else				//else use 12 cycle to get 12 bit ADC data
    begin
        if(iCLK)
        begin
            if (m_cont == 4)
                adc_data[11]    <=  iDOUT;
            else if (m_cont == 5)
                adc_data[10]    <=  iDOUT;
            else if (m_cont == 6)
                adc_data[9]     <=  iDOUT;
            else if (m_cont == 7)
                adc_data[8]     <=  iDOUT;
            else if (m_cont == 8)
                adc_data[7]     <=  iDOUT;
            else if (m_cont == 9)
                adc_data[6]     <=  iDOUT;
            else if (m_cont == 10)
                adc_data[5]     <=  iDOUT;
            else if (m_cont == 11)
                adc_data[4]     <=  iDOUT;
            else if (m_cont == 12)
                adc_data[3]     <=  iDOUT;
            else if (m_cont == 13)
                adc_data[2]     <=  iDOUT;
            else if (m_cont == 14)
                adc_data[1]     <=  iDOUT;
            else if (m_cont == 15)
                adc_data[0]     <=  iDOUT;
            else if (m_cont == 1)
                led <=  adc_data[11:4];
        end
    end
end

endmodule