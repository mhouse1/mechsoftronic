/**************************************
* Module: ADC_CTRL
* Date:2014-01-05  ${time}
* Author: Mike     
* modify
* Description: modified version of a DE0 demo file
***************************************/
module ADC_CTRL (   
                    iRST, //key 0
                    iCLK, //SPI Clock
                    iCLK_n, // SPI clock n
                    iGO, //key 1
//                    iCH, //3 bit channel select
                    oLED, //output data
                    
                    oADC_value0,//12 bit digital value
                    oADC_value1,//12 bit digital value
                    oADC_value2,//12 bit digital value
                    oADC_value3,//12 bit digital value
                    oADC_value4,//12 bit digital value
                    oADC_value5,//12 bit digital value
                    oADC_value6,//12 bit digital value
                    oADC_value7,//12 bit digital value
                    
                    
                    oDIN, //ADC_SADDR
                    oCS_n,//ADC_CS_N
                    oSCLK,//ADC_SCLK
                    iDOUT //ADC_SDAT
                );
                    
input               iRST;
input               iCLK;
input               iCLK_n;
input               iGO;
//input   [2:0]       iCH;
output  [7:0]       oLED;

output [11:0]       oADC_value0;
output [11:0]       oADC_value1;
output [11:0]       oADC_value2;
output [11:0]       oADC_value3;
output [11:0]       oADC_value4;
output [11:0]       oADC_value5;
output [11:0]       oADC_value6;
output [11:0]       oADC_value7;





output              oDIN;
output              oCS_n;
output              oSCLK;
input               iDOUT;

reg                 data;
reg                 go_en;
wire    [2:0]       ch_sel;
reg                 sclk;
reg     [3:0]       cont;
reg     [2:0]       read_channel = 4'b0000; //initialize read_channel to start at 0
reg     [3:0]       m_cont;
reg     [11:0]      adc_data;
reg     [7:0]       led;


reg [11:0]   reg_ADC_value0;
reg [11:0]   reg_ADC_value1;
reg [11:0]   reg_ADC_value2;
reg [11:0]   reg_ADC_value3;
reg [11:0]   reg_ADC_value4;
reg [11:0]   reg_ADC_value5;
reg [11:0]   reg_ADC_value6;
reg [11:0]   reg_ADC_value7;

assign  oCS_n       =   ~go_en;
assign  oSCLK       =   (go_en)? iCLK:1;
assign  oDIN        =   data;
assign  ch_sel      =   read_channel;//iCH;
assign  oLED        =   led;


assign  oADC_value0 =   reg_ADC_value0;
assign  oADC_value1 =   reg_ADC_value1;
assign  oADC_value2 =   reg_ADC_value2;
assign  oADC_value3 =   reg_ADC_value3;
assign  oADC_value4 =   reg_ADC_value4;
assign  oADC_value5 =   reg_ADC_value5;
assign  oADC_value6 =   reg_ADC_value6;
assign  oADC_value7 =   reg_ADC_value7;

always@(posedge iGO or negedge iRST)//on key press event
begin
    if(!iRST) //if reset key pressed do not enable ADC
        go_en   <=  0;
    else
    begin
        if(iGO) //if go key pressed enable ADC
            go_en   <=  1;
            //read_channel <= 0; //initialize read_channel to 0
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
                data <= read_channel[2];
                //data    <=  iCH[2];
            else if (cont == 3)
                data <= read_channel[1];
                //data    <=  iCH[1];
            else if (cont == 4)
                data <= read_channel[0];
                //data    <=  iCH[0];
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
    else				//else use 12 cycle to get 12 bit ADC data xD!
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
					begin
					//led <=  {read_channel[2:0],5'b00000}; //verilog concatenation
					 
					 led <= adc_data[7:0];
					 case(read_channel) //send output to corresponding register
					   3'b000: reg_ADC_value0 <= adc_data;
					   3'b001: reg_ADC_value1 <= adc_data;
					   3'b010: reg_ADC_value2 <= adc_data;
					   3'b011: reg_ADC_value3 <= adc_data;
					   3'b100: reg_ADC_value4 <= adc_data;
					   3'b101: reg_ADC_value5 <= adc_data;
					   3'b110: reg_ADC_value6 <= adc_data;
					   3'b111: reg_ADC_value7 <= adc_data;
					   default: reg_ADC_value0 <= 12'b000000000000;// set first channel to 0xBEE if not valid channel
					 endcase
					   
                 read_channel <= read_channel + 1; //set to read next channel :) 
					end;
					
        end
    end
end

endmodule