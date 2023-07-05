                                                                   
library ieee;                                                      
use ieee.std_logic_1164.all;                                       
                                                                   
entity pg_fix_addsub is                                            
  generic(PG_WIDTH: integer;                                       
          PG_PIPELINE: integer;                                    
          PG_DIRECTION: string);                                   
  port(                                                            
       x,y : in std_logic_vector(PG_WIDTH-1 downto 0);             
       z : out std_logic_vector(PG_WIDTH-1 downto 0);              
       clk : in std_logic                                          
  );                                                               
end pg_fix_addsub;                                                 
                                                                   
architecture rtl of pg_fix_addsub is                               
                                                                   
 component lpm_add_sub                                             
  generic (LPM_WIDTH: integer;                                     
	   LPM_PIPELINE: integer;                                    
	   LPM_DIRECTION: string);                                   
  port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);    
      clock: in std_logic;                                         
      result: out std_logic_vector(LPM_WIDTH-1 downto 0));         
  end component;                                                   
                                                                   
begin                                                              
                                                                   
  u1: lpm_add_sub generic map (LPM_WIDTH=>PG_WIDTH,                
                LPM_PIPELINE=>PG_PIPELINE,LPM_DIRECTION=>PG_DIRECTION)
         port map(dataa=>x,datab=>y,clock=>clk,result=>z);         
                                                                   
end rtl;                                                           
                                                                   
library ieee;                                                      
use ieee.std_logic_1164.all;                                       
                                                                   
entity pg_conv_ftol_32_17_8 is         
  port(fixdata : in std_logic_vector(31 downto 0);      
       logdata : out std_logic_vector(16 downto 0);     
       clk : in std_logic);                                        
end pg_conv_ftol_32_17_8;              
                                                                   
architecture rtl of pg_conv_ftol_32_17_8 is 
                                                                   
  component table                                                  
  generic (IN_WIDTH: integer ;                                     
           OUT_WIDTH: integer ;                                    
           TABLE_FILE: string);                                    
  port(indata : in std_logic_vector(IN_WIDTH-1 downto 0);          
      outdata : out std_logic_vector(OUT_WIDTH-1 downto 0);        
      clk : in std_logic);                                         
  end component;                                                   
                                                                   
  component lpm_add_sub                                            
    generic (LPM_WIDTH: integer;                                   
             LPM_DIRECTION: string);                               
    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);  
          result: out std_logic_vector(LPM_WIDTH-1 downto 0));     
  end component;                                                   
                                                                   
  component penc_31_5                        
    port( a : in std_logic_vector(30 downto 0);        
          c : out std_logic_vector(4 downto 0));      
  end component;                                                   
                                                                   
  component shift_ftol_30_10                 
    port( indata : in std_logic_vector(29 downto 0);   
          control : in std_logic_vector(4 downto 0);  
          outdata : out std_logic_vector(9 downto 0));
  end component;                                                   
                                                                   
  signal d1,d0: std_logic_vector(30 downto 0);         
  signal d2: std_logic_vector(30 downto 0);            
  signal d3,d3r: std_logic_vector(30 downto 0);        
  signal one: std_logic_vector(30 downto 0);           
  signal sign: std_logic;                                          
  signal c1: std_logic_vector(4 downto 0);            
  signal d4: std_logic_vector(29 downto 0);            
  signal c2,c3,c4,add: std_logic_vector(4 downto 0);  
  signal d5,d6: std_logic_vector(9 downto 0);         
  signal sign0,sign0r,sign1,sign2,sign3: std_logic;                
  signal nz0,nz1,nz2: std_logic;                                   
                                                                   
begin                                                              
                                                                   
  d1 <=  NOT fixdata(30 downto 0);                     
  one <= "0000000000000000000000000000001";                                              
  u1: lpm_add_sub generic map (LPM_WIDTH=>31,LPM_DIRECTION=>"ADD")
                  port map(result=>d2,dataa=>d1,datab=>one);     
  d0 <= fixdata(30 downto 0);                        
  sign0 <= fixdata(31);                              
                                                                 
  with sign0 select                                              
    d3 <= d0 when '0',                                           
    d2 when others;                                              
                                                                 
  process(clk) begin                                             
    if(clk'event and clk='1') then                               
      d3r <= d3;                                                 
      sign1 <= sign0;                                            
    end if;                                                      
  end process;                                                   
                                                                 
  u2: penc_31_5 port map (a=>d3r,c=>c1);   
  with d3r select                                                
    nz0 <= '0' when "0000000000000000000000000000000",                                  
           '1' when others;                                      
                                                                 
  process(clk) begin                                             
    if(clk'event and clk='1') then                               
      d4 <= d3r(29 downto 0);                        
      c2 <= c1;                                                  
      sign2 <= sign1;                                            
      nz1 <= nz0;                                                
    end if;                                                      
  end process;                                                   
                                                                 
  u3: shift_ftol_30_10                     
            port map (indata=>d4,control=>c2,outdata=>d5);       
                                                                 
  process(clk) begin                                             
    if(clk'event and clk='1') then                               
      d6 <= d5;                                                  
      sign3 <= sign2;                                            
      nz2 <= nz1;                                                
      c3 <= c2;                                                  
    end if;                                                      
  end process;                                                   
                                                                 
  u4: table generic map (IN_WIDTH=>10,OUT_WIDTH=>8,TABLE_FILE=>"ftl0.mif") 
            port map(indata=>d6,outdata=>logdata(7 downto 0),clk=>clk);
                                                                 
  with d6 select                                                 
    add <= "00001" when "1111111111",                          
           "00001" when "1111111110",                        
           "00001" when "1111111101",                        
           "00000" when others;                               
                                                                 
  u5: lpm_add_sub generic map (LPM_WIDTH=>5,LPM_DIRECTION=>"ADD")
                  port map(result=>c4,dataa=>c3,datab=>add);     
                                                                 
  logdata(14 downto 13) <= "00";                    
                                                                 
  process(clk) begin                                             
    if(clk'event and clk='1') then                               
      logdata(16) <= sign3 ;                          
      logdata(15) <= nz2;                             
      logdata(12 downto 8) <= c4;                    
    end if;                                                      
  end process;                                                   
                                                                 
end rtl;                                                         
                                                                    
library ieee;                                                       
use ieee.std_logic_1164.all;                                        
                                                                    
entity table is                                                     
  generic(IN_WIDTH: integer := 7;                                   
       OUT_WIDTH: integer := 5;                                     
       TABLE_FILE: string := "ftol.mif");                         
  port(indata : in std_logic_vector(IN_WIDTH-1 downto 0);           
       outdata : out std_logic_vector(OUT_WIDTH-1 downto 0);        
       clk : in std_logic);                                         
end table;                                                          
                                                                    
architecture rtl of table is                                        
                                                                    
  component lpm_rom                                                 
    generic (LPM_WIDTH: POSITIVE;                                   
             LPM_WIDTHAD: POSITIVE;                                 
          LPM_ADDRESS_CONTROL: STRING;                              
          LPM_FILE: STRING);                                        
   port (address: in std_logic_vector(LPM_WIDTHAD-1 downto 0);      
         outclock: in std_logic;                                    
         q: out std_logic_vector(LPM_WIDTH-1 downto 0));            
  end component;                                                    
                                                                    
begin                                                               
                                                                    
  u1: lpm_rom generic map (LPM_WIDTH=>OUT_WIDTH,                    
                           LPM_WIDTHAD=>IN_WIDTH,                   
                           LPM_ADDRESS_CONTROL=>"UNREGISTERED",   
                           LPM_FILE=>TABLE_FILE)                    
  port map(address=>indata,outclock=>clk,q=>outdata);               
                                                                    
end rtl;                                                            
                                                                    
                                                                    
library ieee;                                                       
use ieee.std_logic_1164.all;                                        
                                                                    
entity penc_31_5 is                             
port( a : in std_logic_vector(30 downto 0);              
      c : out std_logic_vector(4 downto 0));           
end penc_31_5;                                  
                                                                    
architecture rtl of penc_31_5 is                
begin                                                               
                                                                    
  process(a) begin                                                  
    if(a(30)='1') then                                    
      c <= "11110";                                              
    elsif(a(29)='1') then                                      
      c <= "11101";                                             
    elsif(a(28)='1') then                                      
      c <= "11100";                                             
    elsif(a(27)='1') then                                      
      c <= "11011";                                             
    elsif(a(26)='1') then                                      
      c <= "11010";                                             
    elsif(a(25)='1') then                                      
      c <= "11001";                                             
    elsif(a(24)='1') then                                      
      c <= "11000";                                             
    elsif(a(23)='1') then                                      
      c <= "10111";                                             
    elsif(a(22)='1') then                                      
      c <= "10110";                                             
    elsif(a(21)='1') then                                      
      c <= "10101";                                             
    elsif(a(20)='1') then                                      
      c <= "10100";                                             
    elsif(a(19)='1') then                                      
      c <= "10011";                                             
    elsif(a(18)='1') then                                      
      c <= "10010";                                             
    elsif(a(17)='1') then                                      
      c <= "10001";                                             
    elsif(a(16)='1') then                                      
      c <= "10000";                                             
    elsif(a(15)='1') then                                      
      c <= "01111";                                             
    elsif(a(14)='1') then                                      
      c <= "01110";                                             
    elsif(a(13)='1') then                                      
      c <= "01101";                                             
    elsif(a(12)='1') then                                      
      c <= "01100";                                             
    elsif(a(11)='1') then                                      
      c <= "01011";                                             
    elsif(a(10)='1') then                                      
      c <= "01010";                                             
    elsif(a(9)='1') then                                      
      c <= "01001";                                             
    elsif(a(8)='1') then                                      
      c <= "01000";                                             
    elsif(a(7)='1') then                                      
      c <= "00111";                                             
    elsif(a(6)='1') then                                      
      c <= "00110";                                             
    elsif(a(5)='1') then                                      
      c <= "00101";                                             
    elsif(a(4)='1') then                                      
      c <= "00100";                                             
    elsif(a(3)='1') then                                      
      c <= "00011";                                             
    elsif(a(2)='1') then                                      
      c <= "00010";                                             
    elsif(a(1)='1') then                                      
      c <= "00001";                                             
    else                                                            
      c <= "00000";                                               
    end if;                                                         
  end process;                                                      
                                                                    
end rtl;                                                            
                                                                    
library ieee;                                                       
use ieee.std_logic_1164.all;                                        
                                                                    
entity shift_ftol_30_10 is                       
  port( indata : in std_logic_vector(29 downto 0);       
        control : in std_logic_vector(4 downto 0);     
        outdata : out std_logic_vector(9 downto 0));   
end shift_ftol_30_10;                            
                                                                    
architecture rtl of shift_ftol_30_10 is          
                                                                    
  component lpm_clshift                                             
    generic (LPM_WIDTH : POSITIVE;                                  
             LPM_WIDTHDIST : POSITIVE);                             
    port(data : in std_logic_vector(LPM_WIDTH-1 downto 0);          
         distance : in std_logic_vector(LPM_WIDTHDIST-1 downto 0);  
         direction : in std_logic;                                  
         result : out std_logic_vector(LPM_WIDTH-1 downto 0));      
  end component;                                                    
                                                                    
  signal d0 : std_logic_vector(39 downto 0);    
  signal o0 : std_logic_vector(39 downto 0);    
  signal one : std_logic;                                           
                                                                    
begin                                                               
                                                                    
  one <= '1';                                                       
  d0 <= indata & "0000000000";                                        
  u1: lpm_clshift generic map (LPM_WIDTH=>40,LPM_WIDTHDIST=>5)
         port map (data=>d0,distance=>control,result=>o0,direction=>one);
  outdata <= o0(9 downto 0);                           
                                                                    
end rtl;                                                            
                                                            
library ieee;                                               
use ieee.std_logic_1164.all;                                
                                                            
entity pg_pdelay_8 is                               
  generic (PG_WIDTH: integer);                              
  port( x : in std_logic_vector(PG_WIDTH-1 downto 0);       
	   y : out std_logic_vector(PG_WIDTH-1 downto 0);     
	   clk : in std_logic);                               
end pg_pdelay_8;                                    
                                                            
architecture rtl of pg_pdelay_8 is                  
                                                            
  signal x0 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x1 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x2 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x3 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x4 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x5 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x6 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x7 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x8 : std_logic_vector(PG_WIDTH-1 downto 0);   
                                                            
begin                                                       
                                                            
  x0 <= x;                                                  
                                                            
  process(clk) begin                                        
    if(clk'event and clk='1') then                          
      x8 <= x7;                                   
      x7 <= x6;                                   
      x6 <= x5;                                   
      x5 <= x4;                                   
      x4 <= x3;                                   
      x3 <= x2;                                   
      x2 <= x1;                                   
      x1 <= x0;                                   
    end if;                                                 
  end process;                                              
                                                            
  y <= x8;                                           
                                                            
end rtl;                                                    
                                                           
library ieee;                                              
use ieee.std_logic_1164.all;                               
                                                           
entity pg_log_shift_1 is                          
  generic (PG_WIDTH: integer);                             
  port( x : in std_logic_vector(PG_WIDTH-1 downto 0);      
	   y : out std_logic_vector(PG_WIDTH-1 downto 0);    
	   clk : in std_logic);                              
end pg_log_shift_1;                               
                                                           
architecture rtl of pg_log_shift_1 is             
                                                           
begin                                                      
                                                           
  y <= '0' & x(PG_WIDTH-2) & x(PG_WIDTH-4 downto 0) & '0';     
                                                           
end rtl;                                                   
                                                            
library ieee;                                               
use ieee.std_logic_1164.all;                                
                                                            
entity pg_pdelay_5 is                               
  generic (PG_WIDTH: integer);                              
  port( x : in std_logic_vector(PG_WIDTH-1 downto 0);       
	   y : out std_logic_vector(PG_WIDTH-1 downto 0);     
	   clk : in std_logic);                               
end pg_pdelay_5;                                    
                                                            
architecture rtl of pg_pdelay_5 is                  
                                                            
  signal x0 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x1 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x2 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x3 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x4 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x5 : std_logic_vector(PG_WIDTH-1 downto 0);   
                                                            
begin                                                       
                                                            
  x0 <= x;                                                  
                                                            
  process(clk) begin                                        
    if(clk'event and clk='1') then                          
      x5 <= x4;                                   
      x4 <= x3;                                   
      x3 <= x2;                                   
      x2 <= x1;                                   
      x1 <= x0;                                   
    end if;                                                 
  end process;                                              
                                                            
  y <= x5;                                           
                                                            
end rtl;                                                    
                                                                   
library ieee;                                                      
use ieee.std_logic_1164.all;                                       
                                                                   
entity pg_log_unsigned_add_17_8 is            
  port( x,y : in std_logic_vector(16 downto 0);        
        z : out std_logic_vector(16 downto 0);         
        clock : in std_logic);                                     
end pg_log_unsigned_add_17_8;                 
                                                                   
architecture rtl of pg_log_unsigned_add_17_8 is
                                                                   
  component lpm_add_sub                                            
    generic (LPM_WIDTH: integer;                                   
             LPM_DIRECTION: string);                               
    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);  
          result: out std_logic_vector(LPM_WIDTH-1 downto 0));     
  end component;                                                   
                                                                   
  component table                                                  
    generic (IN_WIDTH: integer ;                                   
	     OUT_WIDTH: integer ;                                      
	     TABLE_FILE: string);                                      
  port(indata : in std_logic_vector(IN_WIDTH-1 downto 0);          
       outdata : out std_logic_vector(OUT_WIDTH-1 downto 0);       
       clk : in std_logic);                                        
  end component;                                                   
                                                                   
  signal x1,y1,xy,yx : std_logic_vector(16 downto 0);  
  signal x2,x3,x4 : std_logic_vector(15 downto 0);     
  signal d0,d1,d4 : std_logic_vector(15 downto 0);     
  signal df0,df1 : std_logic;                                      
  signal z0 : std_logic_vector(15 downto 0);           
  signal d2 : std_logic_vector(8 downto 0);          
  signal sign0,sign1,sign2 : std_logic;                            
  signal signxy : std_logic_vector(1 downto 0);                    
                                                                   
begin                                                              
                                                                   
  x1 <= '0' & x(15 downto 0);                          
  y1 <= '0' & y(15 downto 0);                          
  u1: lpm_add_sub generic map(LPM_WIDTH=>17,LPM_DIRECTION=>"SUB")
       port map(dataa=>x1,datab=>y1,result=>xy);                   
  u2: lpm_add_sub generic map(LPM_WIDTH=>17,LPM_DIRECTION=>"SUB")
       port map(dataa=>y1,datab=>x1,result=>yx);                   
                                                                   
  x2 <= x(15 downto 0) when yx(16)='1' else y(15 downto 0);
  d0 <= xy(15 downto 0) when yx(16)='1' else yx(15 downto 0);
                                                                   
  signxy <= x(16)&y(16);                    
  with signxy select                                               
    sign0 <= y(16) when "01",                        
             x(16) when others;                        
                                                                   
  process(clock) begin                                             
    if(clock'event and clock='1') then                             
      x3 <= x2;                                                    
      d1 <= d0;                                                    
      sign1 <= sign0;                                              
    end if;                                                        
  end process;                                                     
                                                                   
  df0 <= '1' when d1(15 downto 12)="0000" else '0';
  u3: table generic map(IN_WIDTH=>12,OUT_WIDTH=>9,TABLE_FILE=>"add0.mif")
            port map(indata=>d1(11 downto 0),outdata=>d2,clk=>clock);
                                                                   
  process(clock) begin                                             
    if(clock'event and clock='1') then                             
      df1 <= df0;                                                  
      x4 <= x3;                                                    
      sign2 <= sign1;                                              
    end if;                                                        
  end process;                                                     
                                                                   
  d4(8 downto 0) <= d2 when (df1 = '1') else "000000000";
  d4(15 downto 9) <= "0000000";           
                                                                   
  u4: lpm_add_sub generic map(LPM_WIDTH=>16,LPM_DIRECTION=>"ADD")
       port map(dataa=>x4,datab=>d4,result=>z0);                   
                                                                   
  process(clock) begin                                             
    if(clock'event and clock='1') then                             
      z(15 downto 0) <= z0;                            
      z(16) <= sign2;                                  
    end if;                                                        
  end process;                                                     
                                                                   
end rtl;                                                           
                                                           
library ieee;                                              
use ieee.std_logic_1164.all;                               
                                                           
entity pg_log_shift_m1 is                          
  generic (PG_WIDTH: integer);                             
  port( x : in std_logic_vector(PG_WIDTH-1 downto 0);      
	   y : out std_logic_vector(PG_WIDTH-1 downto 0);    
	   clk : in std_logic);                              
end pg_log_shift_m1;                               
                                                           
architecture rtl of pg_log_shift_m1 is             
                                                           
begin                                                      
                                                           
  y <= '0' & x(PG_WIDTH-2) & '0' & x(PG_WIDTH-3 downto 1);   
                                                           
end rtl;                                                   
                                                                   
library ieee;                                                      
use ieee.std_logic_1164.all;                                       
                                                                   
entity pg_log_muldiv_1 is                                  
  generic (PG_WIDTH: integer;                                      
           PG_MULDIV: string);                                     
  port( x,y : in std_logic_vector(PG_WIDTH-1 downto 0);            
	   z : out std_logic_vector(PG_WIDTH-1 downto 0);            
	   clk : in std_logic);                                      
end pg_log_muldiv_1;                                       
                                                                   
architecture rtl of pg_log_muldiv_1 is                   
                                                                   
  component lpm_add_sub                                            
    generic (LPM_WIDTH: integer;                                   
             LPM_PIPELINE: integer;                                
             LPM_DIRECTION: string);                               
    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);  
          clock: in std_logic;                                     
          result: out std_logic_vector(LPM_WIDTH-1 downto 0));     
  end component;                                                   
                                                                   
 signal dataa,datab,result: std_logic_vector(PG_WIDTH-3 downto 0); 
                                                                   
begin                                                              
                                                                   
  process(clk) begin                                              
    if(clk'event and clk='1') then                               
      z(PG_WIDTH-1) <= x(PG_WIDTH-1) xor y(PG_WIDTH-1);            
      z(PG_WIDTH-2) <= x(PG_WIDTH-2) and y(PG_WIDTH-2);            
    end if;                                                        
  end process;                                                     
                                                                   
  dataa <= x(PG_WIDTH-3 downto 0);                                 
  datab <= y(PG_WIDTH-3 downto 0);                                 
                                                                   
  u1: lpm_add_sub generic map(LPM_WIDTH=>PG_WIDTH-2,LPM_PIPELINE=>1,
                              LPM_DIRECTION=>PG_MULDIV)           
       port map(dataa=>dataa,datab=>datab,clock=>clk,result=>result);
                                                                   
  z(PG_WIDTH-3 downto 0) <= result;                                
                                                                   
end rtl;                                                           
                                                            
library ieee;                                               
use ieee.std_logic_1164.all;                                
                                                            
entity pg_pdelay_12 is                               
  generic (PG_WIDTH: integer);                              
  port( x : in std_logic_vector(PG_WIDTH-1 downto 0);       
	   y : out std_logic_vector(PG_WIDTH-1 downto 0);     
	   clk : in std_logic);                               
end pg_pdelay_12;                                    
                                                            
architecture rtl of pg_pdelay_12 is                  
                                                            
  signal x0 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x1 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x2 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x3 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x4 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x5 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x6 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x7 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x8 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x9 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x10 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x11 : std_logic_vector(PG_WIDTH-1 downto 0);   
  signal x12 : std_logic_vector(PG_WIDTH-1 downto 0);   
                                                            
begin                                                       
                                                            
  x0 <= x;                                                  
                                                            
  process(clk) begin                                        
    if(clk'event and clk='1') then                          
      x12 <= x11;                                   
      x11 <= x10;                                   
      x10 <= x9;                                   
      x9 <= x8;                                   
      x8 <= x7;                                   
      x7 <= x6;                                   
      x6 <= x5;                                   
      x5 <= x4;                                   
      x4 <= x3;                                   
      x3 <= x2;                                   
      x2 <= x1;                                   
      x1 <= x0;                                   
    end if;                                                 
  end process;                                              
                                                            
  y <= x12;                                           
                                                            
end rtl;                                                    
                                                                   
library ieee;                                                      
use ieee.std_logic_1164.all;                                       
                                                                   
entity pg_conv_ltof_17_8_57 is        
  port(logdata : in std_logic_vector(16 downto 0);
       fixdata : out std_logic_vector(56 downto 0);
       clk : in std_logic);                                        
end pg_conv_ltof_17_8_57;             
                                                                   
architecture rtl of pg_conv_ltof_17_8_57 is 
                                                                   
  component table                                                  
    generic (IN_WIDTH: integer ;                                   
             OUT_WIDTH: integer ;                                  
             TABLE_FILE: string);                                  
    port(indata : in std_logic_vector(IN_WIDTH-1 downto 0);        
         outdata : out std_logic_vector(OUT_WIDTH-1 downto 0);     
         clk : in std_logic);                                      
  end component;                                                   
                                                                   
  component shift_ltof_9_56                 
    port( indata : in std_logic_vector(8 downto 0);   
          control : in std_logic_vector(6 downto 0);
          clk : in std_logic;                                      
          outdata : out std_logic_vector(55 downto 0)); 
  end component;                                                   
                                                                   
  signal frac1 : std_logic_vector(7 downto 0);        
  signal frac2 : std_logic_vector(8 downto 0);        
  signal sign1 : std_logic;                                        
  signal nz1 : std_logic;                                          
  signal exp1 : std_logic_vector(6 downto 0);          
  signal fix1 : std_logic_vector(55 downto 0);          
                                                                   
begin                                                              
                                                                   
  process(clk) begin                                               
    if(clk'event and clk='1') then                                 
      sign1 <= logdata(16);                             
      nz1 <= logdata(15);                               
      exp1 <= logdata(14 downto 8);            
    end if;                                                        
  end process;                                                     
                                                                   
  u1: table generic map (IN_WIDTH=>8,OUT_WIDTH=>8,TABLE_FILE=>"ltf0.mif")
          port map(indata=>logdata(7 downto 0),outdata=>frac1,clk=>clk);
                                                                   
  with nz1 select                                                  
    frac2 <= '1' & frac1 when '1',                                 
             "000000000" when others;                                
                                                                   
  u3: shift_ltof_9_56                       
            port map (indata=>frac2,control=>exp1,outdata=>fix1,clk=>clk);
                                                                   
  process(clk) begin                                               
    if(clk'event and clk='1') then                                 
      fixdata(56) <= sign1;                             
      fixdata(55 downto 0) <= fix1;                     
    end if;                                                        
  end process;                                                     
                                                                   
end rtl;                                                           
                                                                    
library ieee;                                                       
use ieee.std_logic_1164.all;                                        
                                                                    
entity shift_ltof_9_56 is                        
  port( indata : in std_logic_vector(8 downto 0);       
        control : in std_logic_vector(6 downto 0);     
        clk : in std_logic;                                         
        outdata : out std_logic_vector(55 downto 0));   
end shift_ltof_9_56;                             
                                                                    
architecture rtl of shift_ltof_9_56 is
                                                                    
  component lpm_clshift                                             
    generic (LPM_WIDTH : POSITIVE;                                  
             LPM_WIDTHDIST : POSITIVE);                             
    port(data : in std_logic_vector(LPM_WIDTH-1 downto 0);          
         distance : in std_logic_vector(LPM_WIDTHDIST-1 downto 0);  
         result : out std_logic_vector(LPM_WIDTH-1 downto 0));      
  end component;                                                    
                                                                    
  signal d0,o0 : std_logic_vector(71 downto 0);          
                                                                    
begin                                                               
                                                                    
  d0 <= "000000000000000000000000000000000000000000000000000000000000000" & indata;                                        
  u1: lpm_clshift generic map (LPM_WIDTH=>72,LPM_WIDTHDIST=>6)
          port map (data=>d0,distance=>control(5 downto 0),result=>o0);
  outdata <= o0(63 downto 8);                
                                                                    
end rtl;                                                            
                                                                    
library ieee;                                                       
use ieee.std_logic_1164.all;                                        
use ieee.std_logic_unsigned.all;                                    
                                                                    
entity pg_fix_accum_57_64_2 is                     
  port (fdata: in std_logic_vector(56 downto 0);      
        sdata0: out std_logic_vector(63 downto 0);     
        sdata1: out std_logic_vector(63 downto 0);     
        run : in std_logic;                                         
        clk : in std_logic);                                        
end pg_fix_accum_57_64_2;                           
                                                                    
architecture rtl of pg_fix_accum_57_64_2 is        
                                                                    
  component lpm_add_sub                                             
    generic (LPM_WIDTH: integer);                                   
    port (dataa,datab: in std_logic_vector(LPM_WIDTH-1 downto 0);   
          cin : in std_logic;                                       
          add_sub : in std_logic;                                   
          cout : out std_logic;                                     
          result: out std_logic_vector(LPM_WIDTH-1 downto 0));      
  end component;                                                    
                                                                    
  signal fx: std_logic_vector(63 downto 0);               
  signal run2,run1 : std_logic;                                   
  signal signx,signx1,cx1,cx2: std_logic;                         
  signal fxl,sxl1,sxl2: std_logic_vector(31 downto 0);            
  signal fxh,fxh1,sxh1,sxh2 : std_logic_vector(31 downto 0);      
  signal sxl20: std_logic_vector(31 downto 0);   
  signal sxh20: std_logic_vector(31 downto 0);   
  signal sxl21: std_logic_vector(31 downto 0);   
  signal sxh21: std_logic_vector(31 downto 0);   
  signal count,count2: std_logic_vector(3 downto 0);              
                                                                  
begin                                                             
                                                                  
  process(clk) begin                                              
    if(clk'event and clk='1') then                                
      run2 <= run1;                                               
      run1 <= run;                                                
    end if;                                                       
  end process;                                                    
                                                                  
                                                                  
  process(clk) begin                                              
    if(clk'event and clk='1') then                                
      if(run1 = '1') then                                         
        if(count = "0001") then                               
          count <= "0000";                                      
        else                                                      
          count <= count + "0001";                              
        end if;                                                   
      else                                                        
        if(run = '1') then                                        
          count <= "0000";                                      
        end if;                                                   
      end if;                                                     
    end if;                                                       
  end process;                                                    
                                                                  
  process(clk) begin                                              
    if(clk'event and clk='1') then                                
      if(run2 = '1') then                                         
        if(count2 = "0001") then                              
          count2 <= "0000";                                     
        else                                                      
          count2 <= count2 + "0001";                            
        end if;                                                   
      else                                                        
        if(run1 = '1') then                                       
          count2 <= "0000";                                     
        end if;                                                   
      end if;                                                     
    end if;                                                       
  end process;                                                    
                                                                  
  fx <= "00000000" & fdata(55 downto 0);                              
  signx <= not fdata(56);                                         
  fxl <= fx(31 downto 0);                                         
  fxh <= fx(63 downto 32);                                        
                                                                  
  u1: lpm_add_sub generic map(LPM_WIDTH=>32)                      
      port map(dataa=>sxl2,                                       
               datab=>fxl,add_sub=>signx,                         
               cin=>fdata(56),                                    
               cout=>cx1,result=>sxl1);                           
                                                                  
  process(clk) begin                                              
    if(clk'event and clk='1') then                                
      cx2 <= cx1;                                                 
      fxh1 <= fxh;                                                
      signx1 <= signx;                                            
    end if;                                                       
  end process;                                                    
                                                                  
  process(clk) begin                                              
    if(clk'event and clk='1') then                                
      if(run1 = '1') then                                         
        if(count = "0000") then                               
          sxl20 <= sxl1;                                    
        end if;                                                   
      else                                                        
        if(run = '1') then                                        
          sxl20 <= "00000000000000000000000000000000";
        end if;                                                   
      end if;                                                     
    end if;                                                       
  end process;                                                    
                                                                  
  sdata0(31 downto 0) <= sxl20;               
                                                                  
  process(clk) begin                                              
    if(clk'event and clk='1') then                                
      if(run1 = '1') then                                         
        if(count = "0001") then                               
          sxl21 <= sxl1;                                    
        end if;                                                   
      else                                                        
        if(run = '1') then                                        
          sxl21 <= "00000000000000000000000000000000";
        end if;                                                   
      end if;                                                     
    end if;                                                       
  end process;                                                    
                                                                  
  sdata1(31 downto 0) <= sxl21;               
  with count select                                             
    sxl2 <= sxl20 when "0000",                                
            sxl21 when "0001",                       
            sxl20 when others;                                   
                                                               
  u2: lpm_add_sub generic map(LPM_WIDTH=>32)                      
      port map(dataa=>sxh2,                                       
               datab=>fxh1,add_sub=>signx1,                       
               cin=>cx2,result=>sxh1);                            
                                                                  
  process(clk) begin                                              
    if(clk'event and clk='1') then                                
      if(run2 = '1') then                                         
        if(count2 = "0000") then                              
          sxh20 <= sxh1;                                    
        end if;                                                   
      else                                                        
        if(run1 = '1') then                                       
          sxh20 <= "00000000000000000000000000000000";
        end if;                                                   
      end if;                                                     
    end if;                                                       
  end process;                                                    
                                                                  
  sdata0(63 downto 32) <= sxh20;        
                                                                  
                                                                  
  process(clk) begin                                              
    if(clk'event and clk='1') then                                
      if(run2 = '1') then                                         
        if(count2 = "0001") then                              
          sxh21 <= sxh1;                                    
        end if;                                                   
      else                                                        
        if(run1 = '1') then                                       
          sxh21 <= "00000000000000000000000000000000";
        end if;                                                   
      end if;                                                     
    end if;                                                       
  end process;                                                    
                                                                  
  sdata1(63 downto 32) <= sxh21;        
                                                                  
  with count2 select                                             
    sxh2 <= sxh20 when "0000",                                 
            sxh21 when "0001",                       
            sxh20 when others;                                   
                                                                    
end rtl;                                                            
