library ieee;                                                  
use ieee.std_logic_1164.all;                                   
                                                               
entity pipeline is                                             
  generic(JDATA_WIDTH : integer := 72);                        
  port(                                                        
    p_jdata : in std_logic_vector(JDATA_WIDTH-1 downto 0);     
    p_run : in std_logic;                                      
    p_we :  in std_logic;                                      
    p_adri : in std_logic_vector(11 downto 0);                 
    p_datai : in std_logic_vector(31 downto 0);                
    p_adro : in std_logic_vector(11 downto 0);                 
    p_datao : out std_logic_vector(31 downto 0);               
    p_runret : out std_logic;                                  
    rst,clk : in std_logic                                     
  );                                                           
end pipeline;                                                  
                                                               
architecture std of pipeline is                                
                                                               
  component pipe                                               
    generic(JDATA_WIDTH : integer );                           
    port(                                                      
      p_jdata: in std_logic_vector(JDATA_WIDTH-1 downto 0);    
      p_run : in std_logic;                                    
      p_we : in std_logic;                                     
      p_adri : in std_logic_vector(3 downto 0);                
      p_adrivp : in std_logic_vector(3 downto 0);              
      p_datai : in std_logic_vector(31 downto 0);              
      p_adro : in std_logic_vector(3 downto 0);                
      p_adrovp : in std_logic_vector(3 downto 0);              
      p_datao : out std_logic_vector(31 downto 0);             
      p_runret : out std_logic;                                
      rst,pclk : in std_logic );                                
  end component;                                               
                                                               
  signal u_adri,u_adro: std_logic_vector(7 downto 0);          
  signal adrivp,adrovp: std_logic_vector(3 downto 0);          
  signal we,runret: std_logic_vector(0 downto 0);
  signal datao: std_logic_vector(31 downto 0);
  signal l_adro: std_logic_vector(3 downto 0);                 
                                                               
begin                                                          
                                                               
  u_adri <= p_adri(11 downto 4);                               
                                                               
  u_adro <= p_adro(11 downto 4);                               
  l_adro <= p_adro(3 downto 0);                                
                                                               
  process(u_adri,p_we) begin                                 
    if(p_we = '1') then                                      
     if(u_adri = "00000000" or u_adri = "00000001" or u_adri = "00000010" ) then
        we(0) <= '1';                                    
      else                                                   
        we(0) <= '0';                                    
      end if;                                                
    else                                                     
      we(0) <= '0';                                      
    end if;                                                  
  end process;                                               
                                                             
  with u_adri select                                           
    adrivp <= "0000" when "00000000", 
              "0001" when "00000001", 
              "0010" when "00000010", 
              "0000" when others;                            
                                                               
  with u_adro select                                           
    adrovp <= "0000" when "00000000", 
              "0001" when "00000001", 
              "0010" when "00000010", 
              "0000" when others;                            
                                                               
  forgen1: for i in 0 to 0 generate           
    upipe: pipe GENERIC MAP(JDATA_WIDTH=>JDATA_WIDTH)          
	      PORT MAP(p_jdata=>p_jdata, p_run=>p_run,               
                 p_we=>we(i),p_adri=>p_adri(3 downto 0),p_adrivp=>adrivp,
	               p_datai=>p_datai,p_adro=>l_adro,p_adrovp=>adrovp, 
	               p_datao=>datao(32*(i+1)-1 downto 32*i), p_runret=>runret(i), 
		       rst=>rst,pclk=>clk);                           
  end generate forgen1;                                        
                                                               
  p_runret <= runret(0);                                       
                                                               
  with u_adro select                                           
    p_datao <= datao(31 downto 0) when "00000000", 
               datao(31 downto 0) when "00000001", 
               datao(31 downto 0) when "00000010", 
               datao(31 downto 0) when others;                  
                                                               
end std;                                               

library ieee;                                                       
use ieee.std_logic_1164.all;                                        
use ieee.std_logic_unsigned.all;                                    
                                                                    
entity pipe is                                                      
  generic(JDATA_WIDTH : integer :=72) ;                             
port(p_jdata : in std_logic_vector(JDATA_WIDTH-1 downto 0);         
     p_run : in std_logic;                                          
     p_we :  in std_logic;                                          
     p_adri : in std_logic_vector(3 downto 0);                      
     p_adrivp : in std_logic_vector(3 downto 0);                    
     p_datai : in std_logic_vector(31 downto 0);                    
     p_adro : in std_logic_vector(3 downto 0);                      
     p_adrovp : in std_logic_vector(3 downto 0);                    
     p_datao : out std_logic_vector(31 downto 0);                   
     p_runret : out std_logic;                                      
     rst,pclk : in std_logic);                                      
end pipe;                                                           
                                                                    
architecture std of pipe is                                         
                                                                    
  component pg_fix_addsub                                          
    generic(PG_WIDTH: integer;                                     
            PG_PIPELINE: integer;                                 
            PG_DIRECTION: string);                                 
    port(x,y : in std_logic_vector(PG_WIDTH-1 downto 0);           
	 z : out std_logic_vector(PG_WIDTH-1 downto 0);              
	 clk : in std_logic);                                        
  end component;                                                   
                                                                   
  component pg_conv_ftol_24_14_5                       
    port(fixdata : in std_logic_vector(23 downto 0);     
         logdata : out std_logic_vector(13 downto 0);    
         clk : in std_logic);                                      
  end component;                                                   
                                                                   
  component pg_pdelay_14                            
    generic (PG_WIDTH: integer);                          
    port( x : in std_logic_vector(PG_WIDTH-1 downto 0);   
	     y : out std_logic_vector(PG_WIDTH-1 downto 0); 
	     clk: in std_logic);                            
  end component;                                          
                                                          
  component pg_log_shift_1                        
    generic (PG_WIDTH: integer);                           
    port( x : in std_logic_vector(PG_WIDTH-1 downto 0);    
	     y : out std_logic_vector(PG_WIDTH-1 downto 0);  
	     clk: in std_logic);                             
  end component;                                           
                                                           
  component pg_log_unsigned_add_14_5                    
    port( x,y : in std_logic_vector(13 downto 0);      
          z : out std_logic_vector(13 downto 0);       
          clock : in std_logic);                                   
  end component;                                                   
                                                                   
  component pg_pdelay_3                            
    generic (PG_WIDTH: integer);                          
    port( x : in std_logic_vector(PG_WIDTH-1 downto 0);   
	     y : out std_logic_vector(PG_WIDTH-1 downto 0); 
	     clk: in std_logic);                            
  end component;                                          
                                                          
  component pg_conv_ftol_14_14_5                       
    port(fixdata : in std_logic_vector(13 downto 0);     
         logdata : out std_logic_vector(13 downto 0);    
         clk : in std_logic);                                      
  end component;                                                   
                                                                   
  component pg_log_muldiv_1                             
    generic (PG_WIDTH : integer;                                 
             PG_MULDIV : string);                                
    port( x,y : in std_logic_vector(PG_WIDTH-1 downto 0);        
	     z : out std_logic_vector(PG_WIDTH-1 downto 0);        
	     clk: in std_logic);                                   
  end component;                                                 
                                                                 
  component pg_log_add_14_5                    
    port( x,y : in std_logic_vector(13 downto 0);      
          z : out std_logic_vector(13 downto 0);       
          clock : in std_logic);                                   
  end component;                                                   
                                                                   
  component pg_pdelay_11                            
    generic (PG_WIDTH: integer);                          
    port( x : in std_logic_vector(PG_WIDTH-1 downto 0);   
	     y : out std_logic_vector(PG_WIDTH-1 downto 0); 
	     clk: in std_logic);                            
  end component;                                          
                                                          
  component pg_pdelay_1                            
    generic (PG_WIDTH: integer);                          
    port( x : in std_logic_vector(PG_WIDTH-1 downto 0);   
	     y : out std_logic_vector(PG_WIDTH-1 downto 0); 
	     clk: in std_logic);                            
  end component;                                          
                                                          
  component pg_wtable_14_5                       
    port(indata  : in std_logic_vector(13 downto 0);     
         outdata : out std_logic_vector(13 downto 0);    
         clk : in std_logic);                                      
  end component;                                                   
                                                                   
  component pg_dwtable_14_5                       
    port(indata  : in std_logic_vector(13 downto 0);     
         outdata : out std_logic_vector(13 downto 0);    
         clk : in std_logic);                                      
  end component;                                                   
                                                                   
  component pg_log_smuldiv_1_14                     
    generic (PG_WIDTH : integer;                                 
             PG_MULDIV : string);                                
    port( x,y : in std_logic_vector(PG_WIDTH-1 downto 0);        
	     z : out std_logic_vector(PG_WIDTH-1 downto 0);        
	     clk: in std_logic);                                   
  end component;                                                 
                                                                 
  component pg_conv_ltof_14_5_57      
    port(logdata : in std_logic_vector(13 downto 0);   
	     fixdata : out std_logic_vector(56 downto 0);  
	     clk : in std_logic);                                     
  end component;                                                  
                                                                  
  component pg_fix_accum_57_64_2                          
    port (fdata: in std_logic_vector(56 downto 0);    
          sdata0: out std_logic_vector(63 downto 0);   
          sdata1: out std_logic_vector(63 downto 0);   
          sdata2: out std_logic_vector(63 downto 0);   
          run: in std_logic;                                        
          clk: in std_logic);                                       
  end component;                                                    
                                                                    
  component pg_pdelay_5                            
    generic (PG_WIDTH: integer);                          
    port( x : in std_logic_vector(PG_WIDTH-1 downto 0);   
	     y : out std_logic_vector(PG_WIDTH-1 downto 0); 
	     clk: in std_logic);                            
  end component;                                          
                                                          
  component pg_pdelay_13                            
    generic (PG_WIDTH: integer);                          
    port( x : in std_logic_vector(PG_WIDTH-1 downto 0);   
	     y : out std_logic_vector(PG_WIDTH-1 downto 0); 
	     clk: in std_logic);                            
  end component;                                          
                                                          
  component pg_pdelay_2                            
    generic (PG_WIDTH: integer);                          
    port( x : in std_logic_vector(PG_WIDTH-1 downto 0);   
	     y : out std_logic_vector(PG_WIDTH-1 downto 0); 
	     clk: in std_logic);                            
  end component;                                          
                                                          
  signal run: std_logic_vector(25 downto 0);                        
  signal xi: std_logic_vector(23 downto 0);    
  signal xj: std_logic_vector(23 downto 0);    
  signal yi: std_logic_vector(23 downto 0);    
  signal yj: std_logic_vector(23 downto 0);    
  signal zi: std_logic_vector(23 downto 0);    
  signal zj: std_logic_vector(23 downto 0);    
  signal xij: std_logic_vector(23 downto 0);    
  signal yij: std_logic_vector(23 downto 0);    
  signal zij: std_logic_vector(23 downto 0);    
  signal dx: std_logic_vector(13 downto 0);    
  signal dy: std_logic_vector(13 downto 0);    
  signal dz: std_logic_vector(13 downto 0);    
  signal x2: std_logic_vector(13 downto 0);           
  signal y2: std_logic_vector(13 downto 0);           
  signal z2: std_logic_vector(13 downto 0);    
  signal x2y2: std_logic_vector(13 downto 0);           
  signal z2r: std_logic_vector(13 downto 0);           
  signal vxi: std_logic_vector(13 downto 0);    
  signal vxj: std_logic_vector(13 downto 0);    
  signal vyi: std_logic_vector(13 downto 0);    
  signal vyj: std_logic_vector(13 downto 0);    
  signal vzi: std_logic_vector(13 downto 0);    
  signal vzj: std_logic_vector(13 downto 0);    
  signal vxij: std_logic_vector(13 downto 0);    
  signal vyij: std_logic_vector(13 downto 0);    
  signal vzij: std_logic_vector(13 downto 0);    
  signal dvx: std_logic_vector(13 downto 0);    
  signal dvy: std_logic_vector(13 downto 0);    
  signal dvz: std_logic_vector(13 downto 0);    
  signal vxx: std_logic_vector(13 downto 0);           
  signal vyy: std_logic_vector(13 downto 0);           
  signal vzz: std_logic_vector(13 downto 0);    
  signal vxy: std_logic_vector(13 downto 0);           
  signal vzzr: std_logic_vector(13 downto 0);           
  signal hi: std_logic_vector(13 downto 0);    
  signal hir: std_logic_vector(13 downto 0);    
  signal hii2: std_logic_vector(13 downto 0);    
  signal hii3: std_logic_vector(13 downto 0);    
  signal hii2r: std_logic_vector(13 downto 0);    
  signal r2: std_logic_vector(13 downto 0);    
  signal rhi2: std_logic_vector(13 downto 0);    
  signal wi: std_logic_vector(13 downto 0);    
  signal hii3r: std_logic_vector(13 downto 0);    
  signal dwi: std_logic_vector(13 downto 0);    
  signal hii5: std_logic_vector(13 downto 0);    
  signal hj: std_logic_vector(13 downto 0);    
  signal hjr: std_logic_vector(13 downto 0);    
  signal hj2: std_logic_vector(13 downto 0);    
  signal hj3: std_logic_vector(13 downto 0);    
  signal hj2r: std_logic_vector(13 downto 0);    
  signal rhj2: std_logic_vector(13 downto 0);    
  signal wj: std_logic_vector(13 downto 0);    
  signal hj3r: std_logic_vector(13 downto 0);    
  signal dwj: std_logic_vector(13 downto 0);    
  signal hj5: std_logic_vector(13 downto 0);    
  signal whi: std_logic_vector(13 downto 0);           
  signal whj: std_logic_vector(13 downto 0);           
  signal dwhi: std_logic_vector(13 downto 0);           
  signal dwhj: std_logic_vector(13 downto 0);           
  signal w: std_logic_vector(13 downto 0);    
  signal rhoshift: std_logic_vector(13 downto 0);    
  signal wrhos: std_logic_vector(13 downto 0);    
  signal frho: std_logic_vector(56 downto 0);    
  signal srho0: std_logic_vector(63 downto 0);    
  signal srho1: std_logic_vector(63 downto 0);    
  signal srho2: std_logic_vector(63 downto 0);    
  signal vdotr: std_logic_vector(13 downto 0);    
  signal vdotrr: std_logic_vector(13 downto 0);    
  signal dw: std_logic_vector(13 downto 0);    
  signal vrdw0: std_logic_vector(13 downto 0);    
  signal dshift: std_logic_vector(13 downto 0);    
  signal vrdw: std_logic_vector(13 downto 0);    
  signal fdivv: std_logic_vector(56 downto 0);    
  signal sdivv0: std_logic_vector(63 downto 0);    
  signal sdivv1: std_logic_vector(63 downto 0);    
  signal sdivv2: std_logic_vector(63 downto 0);    
  signal pj: std_logic_vector(13 downto 0);    
  signal pi: std_logic_vector(13 downto 0);    
  signal pir: std_logic_vector(13 downto 0);    
  signal pjr: std_logic_vector(13 downto 0);    
  signal qj: std_logic_vector(13 downto 0);    
  signal qi: std_logic_vector(13 downto 0);    
  signal qir: std_logic_vector(13 downto 0);           
  signal qjr: std_logic_vector(13 downto 0);           
  signal pij: std_logic_vector(13 downto 0);           
  signal qij: std_logic_vector(13 downto 0);           
  signal avij: std_logic_vector(13 downto 0);    
  signal avdw0: std_logic_vector(13 downto 0);    
  signal fshift: std_logic_vector(13 downto 0);    
  signal dxr: std_logic_vector(13 downto 0);    
  signal avdw: std_logic_vector(13 downto 0);    
  signal dyr: std_logic_vector(13 downto 0);    
  signal dzr: std_logic_vector(13 downto 0);    
  signal avdwx: std_logic_vector(13 downto 0);    
  signal avdwy: std_logic_vector(13 downto 0);    
  signal avdwz: std_logic_vector(13 downto 0);    
  signal ax: std_logic_vector(56 downto 0);    
  signal sax0: std_logic_vector(63 downto 0);    
  signal sax1: std_logic_vector(63 downto 0);    
  signal sax2: std_logic_vector(63 downto 0);    
  signal ay: std_logic_vector(56 downto 0);    
  signal say0: std_logic_vector(63 downto 0);    
  signal say1: std_logic_vector(63 downto 0);    
  signal say2: std_logic_vector(63 downto 0);    
  signal az: std_logic_vector(56 downto 0);    
  signal saz0: std_logic_vector(63 downto 0);    
  signal saz1: std_logic_vector(63 downto 0);    
  signal saz2: std_logic_vector(63 downto 0);    
  signal ushift: std_logic_vector(13 downto 0);    
  signal vdotrrr: std_logic_vector(13 downto 0);    
  signal avdw1: std_logic_vector(13 downto 0);    
  signal dudt0: std_logic_vector(13 downto 0);    
  signal fdudt: std_logic_vector(56 downto 0);    
  signal sdudt0: std_logic_vector(63 downto 0);    
  signal sdudt1: std_logic_vector(63 downto 0);    
  signal sdudt2: std_logic_vector(63 downto 0);    
  signal xi0: std_logic_vector(23 downto 0);
  signal xi1: std_logic_vector(23 downto 0);
  signal xi2: std_logic_vector(23 downto 0);
  signal yi0: std_logic_vector(23 downto 0);
  signal yi1: std_logic_vector(23 downto 0);
  signal yi2: std_logic_vector(23 downto 0);
  signal zi0: std_logic_vector(23 downto 0);
  signal zi1: std_logic_vector(23 downto 0);
  signal zi2: std_logic_vector(23 downto 0);
  signal vxi0: std_logic_vector(13 downto 0);
  signal vxi1: std_logic_vector(13 downto 0);
  signal vxi2: std_logic_vector(13 downto 0);
  signal vyi0: std_logic_vector(13 downto 0);
  signal vyi1: std_logic_vector(13 downto 0);
  signal vyi2: std_logic_vector(13 downto 0);
  signal vzi0: std_logic_vector(13 downto 0);
  signal vzi1: std_logic_vector(13 downto 0);
  signal vzi2: std_logic_vector(13 downto 0);
  signal pi0: std_logic_vector(13 downto 0);
  signal pi1: std_logic_vector(13 downto 0);
  signal pi2: std_logic_vector(13 downto 0);
  signal qi0: std_logic_vector(13 downto 0);
  signal qi1: std_logic_vector(13 downto 0);
  signal qi2: std_logic_vector(13 downto 0);
  signal hi0: std_logic_vector(13 downto 0);
  signal hi1: std_logic_vector(13 downto 0);
  signal hi2: std_logic_vector(13 downto 0);
  signal vmp_phase: std_logic_vector(3 downto 0);                 
  signal irun: std_logic_vector(2 downto 0);      
  signal jdata1: std_logic_vector(JDATA_WIDTH-1 downto 0);      
  signal jdata2: std_logic_vector(JDATA_WIDTH-1 downto 0);      
                                                                    
begin                                                               
                                                                    
  rhoshift(13 downto 0) <= "01000100000000";
  dshift(13 downto 0) <= "01011000000000";
  fshift(13 downto 0) <= "01011100000000";
  ushift(13 downto 0) <= "01100000000000";

  process(pclk) begin
    if(pclk'event and pclk='1') then
      jdata1 <= p_jdata;            
      jdata2 <= jdata1;            
    end if;
  end process;
              
  process(pclk) begin
    if(pclk'event and pclk='1') then
      if(vmp_phase = "0000") then
        xj(23 downto 0) <= jdata2(23 downto 0);
        yj(23 downto 0) <= jdata2(47 downto 24);
        zj(23 downto 0) <= jdata1(23 downto 0);
        vxj(13 downto 0) <= jdata1(37 downto 24);
        vyj(13 downto 0) <= jdata1(51 downto 38);
        vzj(13 downto 0) <= p_jdata(13 downto 0);
        hj(13 downto 0) <= p_jdata(27 downto 14);
        pj(13 downto 0) <= p_jdata(41 downto 28);
        qj(13 downto 0) <= p_jdata(55 downto 42);
      end if;
    end if;
  end process;

  process(pclk) begin
    if(pclk'event and pclk='1') then
      if(p_we ='1') then
        if(p_adri = "0000") then
          if(p_adrivp = "0000") then
            xi0 <=  p_datai(23 downto 0);
          elsif(p_adrivp = "0001") then
            xi1 <=  p_datai(23 downto 0);
          elsif(p_adrivp = "0010") then
            xi2 <=  p_datai(23 downto 0);
          end if;
        elsif(p_adri = "0001") then
          if(p_adrivp = "0000") then
            yi0 <=  p_datai(23 downto 0);
          elsif(p_adrivp = "0001") then
            yi1 <=  p_datai(23 downto 0);
          elsif(p_adrivp = "0010") then
            yi2 <=  p_datai(23 downto 0);
          end if;
        elsif(p_adri = "0010") then
          if(p_adrivp = "0000") then
            zi0 <=  p_datai(23 downto 0);
          elsif(p_adrivp = "0001") then
            zi1 <=  p_datai(23 downto 0);
          elsif(p_adrivp = "0010") then
            zi2 <=  p_datai(23 downto 0);
          end if;
        elsif(p_adri = "0011") then
          if(p_adrivp = "0000") then
            vxi0 <=  p_datai(13 downto 0);
          elsif(p_adrivp = "0001") then
            vxi1 <=  p_datai(13 downto 0);
          elsif(p_adrivp = "0010") then
            vxi2 <=  p_datai(13 downto 0);
          end if;
        elsif(p_adri = "0100") then
          if(p_adrivp = "0000") then
            vyi0 <=  p_datai(13 downto 0);
          elsif(p_adrivp = "0001") then
            vyi1 <=  p_datai(13 downto 0);
          elsif(p_adrivp = "0010") then
            vyi2 <=  p_datai(13 downto 0);
          end if;
        elsif(p_adri = "0101") then
          if(p_adrivp = "0000") then
            vzi0 <=  p_datai(13 downto 0);
          elsif(p_adrivp = "0001") then
            vzi1 <=  p_datai(13 downto 0);
          elsif(p_adrivp = "0010") then
            vzi2 <=  p_datai(13 downto 0);
          end if;
        elsif(p_adri = "0110") then
          if(p_adrivp = "0000") then
            pi0 <=  p_datai(13 downto 0);
          elsif(p_adrivp = "0001") then
            pi1 <=  p_datai(13 downto 0);
          elsif(p_adrivp = "0010") then
            pi2 <=  p_datai(13 downto 0);
          end if;
        elsif(p_adri = "0111") then
          if(p_adrivp = "0000") then
            qi0 <=  p_datai(13 downto 0);
          elsif(p_adrivp = "0001") then
            qi1 <=  p_datai(13 downto 0);
          elsif(p_adrivp = "0010") then
            qi2 <=  p_datai(13 downto 0);
          end if;
        elsif(p_adri = "1000") then
          if(p_adrivp = "0000") then
            hi0 <=  p_datai(13 downto 0);
          elsif(p_adrivp = "0001") then
            hi1 <=  p_datai(13 downto 0);
          elsif(p_adrivp = "0010") then
            hi2 <=  p_datai(13 downto 0);
          end if;
        end if;
      end if;
    end if;
  end process;

  process(pclk) begin
    if(pclk'event and pclk='1') then
      if(vmp_phase = "0000") then
        xi <= xi0;
        yi <= yi0;
        zi <= zi0;
        vxi <= vxi0;
        vyi <= vyi0;
        vzi <= vzi0;
        pi <= pi0;
        qi <= qi0;
        hi <= hi0;
      elsif(vmp_phase = "0001") then
        xi <= xi1;
        yi <= yi1;
        zi <= zi1;
        vxi <= vxi1;
        vyi <= vyi1;
        vzi <= vzi1;
        pi <= pi1;
        qi <= qi1;
        hi <= hi1;
      elsif(vmp_phase = "0010") then
        xi <= xi2;
        yi <= yi2;
        zi <= zi2;
        vxi <= vxi2;
        vyi <= vyi2;
        vzi <= vzi2;
        pi <= pi2;
        qi <= qi2;
        hi <= hi2;
      end if;
    end if;
  end process;

  process(pclk) begin                                 
    if(pclk'event and pclk='1') then                  
      irun(0) <= p_run;                               
      irun(1) <= irun(0);                   
      irun(2) <= irun(1);                   
    end if;                                           
  end process;                                        
                                                      
  process(pclk) begin                                 
    if(pclk'event and pclk='1') then                  
      if(irun(2) = '1') then           
        if(vmp_phase = "0010") then              
          vmp_phase <= "0000";                      
        else                                          
          vmp_phase <= vmp_phase + "0001";          
        end if;                                       
      else                                            
        if(p_run = '1') then                          
          vmp_phase <= "0000";                      
        end if;                                       
      end if;                                         
    end if;                                           
  end process;                                        
                                                      
  process(pclk) begin                                               
    if(pclk'event and pclk='1') then                                
      run(0) <= p_run;                                              
      for i in 0 to 24 loop                                         
        run(i+1) <= run(i);                                         
      end loop;                                                     
      p_runret <= run(25);                                          
    end if;                                                         
  end process;                                                      
                                                                    
  u0: pg_fix_addsub generic map (PG_WIDTH=>24,PG_PIPELINE=>1,     
                              PG_DIRECTION=>"SUB")                
               port map (x=>xi,y=>xj,z=>xij,clk=>pclk);             
                                                                   
  u1: pg_fix_addsub generic map (PG_WIDTH=>24,PG_PIPELINE=>1,     
                              PG_DIRECTION=>"SUB")                
               port map (x=>yi,y=>yj,z=>yij,clk=>pclk);             
                                                                   
  u2: pg_fix_addsub generic map (PG_WIDTH=>24,PG_PIPELINE=>1,     
                              PG_DIRECTION=>"SUB")                
               port map (x=>zi,y=>zj,z=>zij,clk=>pclk);             
                                                                   
  u3: pg_conv_ftol_24_14_5   port map (fixdata=>xij,logdata=>dx,clk=>pclk);                 
                                                                     
  u4: pg_conv_ftol_24_14_5   port map (fixdata=>yij,logdata=>dy,clk=>pclk);                 
                                                                     
  u5: pg_conv_ftol_24_14_5   port map (fixdata=>zij,logdata=>dz,clk=>pclk);                 
                                                                     
  u6: pg_pdelay_14 generic map(PG_WIDTH=>14)  
                    port map(x=>dx,y=>dxr,clk=>pclk); 
                                                          
  u7: pg_pdelay_14 generic map(PG_WIDTH=>14)  
                    port map(x=>dy,y=>dyr,clk=>pclk); 
                                                          
  u8: pg_pdelay_14 generic map(PG_WIDTH=>14)  
                    port map(x=>dz,y=>dzr,clk=>pclk); 
                                                          
  u9: pg_log_shift_1 generic map(PG_WIDTH=>14)       
                    port map(x=>dx,y=>x2,clk=>pclk);         
                                                                  
  u10: pg_log_shift_1 generic map(PG_WIDTH=>14)       
                    port map(x=>dy,y=>y2,clk=>pclk);         
                                                                  
  u11: pg_log_shift_1 generic map(PG_WIDTH=>14)       
                    port map(x=>dz,y=>z2,clk=>pclk);         
                                                                  
  u12: pg_log_unsigned_add_14_5      
            port map(x=>x2,y=>y2,z=>x2y2,clock=>pclk);       
                                                                   
  u13: pg_pdelay_3 generic map(PG_WIDTH=>14)  
                    port map(x=>z2,y=>z2r,clk=>pclk); 
                                                          
  u14: pg_log_unsigned_add_14_5      
            port map(x=>x2y2,y=>z2r,z=>r2,clock=>pclk);       
                                                                   
  u15: pg_fix_addsub generic map (PG_WIDTH=>14,PG_PIPELINE=>1,     
                              PG_DIRECTION=>"SUB")                
               port map (x=>vxi,y=>vxj,z=>vxij,clk=>pclk);             
                                                                   
  u16: pg_fix_addsub generic map (PG_WIDTH=>14,PG_PIPELINE=>1,     
                              PG_DIRECTION=>"SUB")                
               port map (x=>vyi,y=>vyj,z=>vyij,clk=>pclk);             
                                                                   
  u17: pg_fix_addsub generic map (PG_WIDTH=>14,PG_PIPELINE=>1,     
                              PG_DIRECTION=>"SUB")                
               port map (x=>vzi,y=>vzj,z=>vzij,clk=>pclk);             
                                                                   
  u18: pg_conv_ftol_14_14_5   port map (fixdata=>vxij,logdata=>dvx,clk=>pclk);                 
                                                                     
  u19: pg_conv_ftol_14_14_5   port map (fixdata=>vyij,logdata=>dvy,clk=>pclk);                 
                                                                     
  u20: pg_conv_ftol_14_14_5   port map (fixdata=>vzij,logdata=>dvz,clk=>pclk);                 
                                                                     
  u21: pg_log_muldiv_1 generic map(PG_WIDTH=>14,
                                PG_MULDIV=>"ADD")
                    port map(x=>dvx,y=>dx,z=>vxx,clk=>pclk);
                                                                  
  u22: pg_log_muldiv_1 generic map(PG_WIDTH=>14,
                                PG_MULDIV=>"ADD")
                    port map(x=>dvy,y=>dy,z=>vyy,clk=>pclk);
                                                                  
  u23: pg_log_muldiv_1 generic map(PG_WIDTH=>14,
                                PG_MULDIV=>"ADD")
                    port map(x=>dvz,y=>dz,z=>vzz,clk=>pclk);
                                                                  
  u24: pg_log_add_14_5               
            port map(x=>vxx,y=>vyy,z=>vxy,clock=>pclk);       
                                                                   
  u25: pg_pdelay_3 generic map(PG_WIDTH=>14)  
                    port map(x=>vzz,y=>vzzr,clk=>pclk); 
                                                          
  u26: pg_log_add_14_5               
            port map(x=>vxy,y=>vzzr,z=>vdotr,clock=>pclk);       
                                                                   
  u27: pg_pdelay_11 generic map(PG_WIDTH=>14)  
                    port map(x=>hi,y=>hir,clk=>pclk); 
                                                          
  u28: pg_log_shift_1 generic map(PG_WIDTH=>14)       
                    port map(x=>hir,y=>hii2,clk=>pclk);         
                                                                  
  u29: pg_log_muldiv_1 generic map(PG_WIDTH=>14,
                                PG_MULDIV=>"ADD")
                    port map(x=>hir,y=>hii2,z=>hii3,clk=>pclk);
                                                                  
  u30: pg_pdelay_1 generic map(PG_WIDTH=>14)  
                    port map(x=>hii2,y=>hii2r,clk=>pclk); 
                                                          
  u31: pg_log_muldiv_1 generic map(PG_WIDTH=>14,
                                PG_MULDIV=>"ADD")
                    port map(x=>hii3,y=>hii2r,z=>hii5,clk=>pclk);
                                                                  
  u32: pg_pdelay_1 generic map(PG_WIDTH=>14)  
                    port map(x=>hii3,y=>hii3r,clk=>pclk); 
                                                          
  u33: pg_log_muldiv_1 generic map(PG_WIDTH=>14,
                                PG_MULDIV=>"SUB")
                    port map(x=>r2,y=>hii2,z=>rhi2,clk=>pclk);
                                                                  
  u34: pg_wtable_14_5   port map (indata=>rhi2,outdata=>wi,clk=>pclk);                  
                                                                     
  u35: pg_log_muldiv_1 generic map(PG_WIDTH=>14,
                                PG_MULDIV=>"SUB")
                    port map(x=>wi,y=>hii3r,z=>whi,clk=>pclk);
                                                                  
  u36: pg_dwtable_14_5   port map (indata=>rhi2,outdata=>dwi,clk=>pclk);                  
                                                                     
  u37: pg_log_muldiv_1 generic map(PG_WIDTH=>14,
                                PG_MULDIV=>"SUB")
                    port map(x=>dwi,y=>hii5,z=>dwhi,clk=>pclk);
                                                                  
  u38: pg_pdelay_11 generic map(PG_WIDTH=>14)  
                    port map(x=>hj,y=>hjr,clk=>pclk); 
                                                          
  u39: pg_log_shift_1 generic map(PG_WIDTH=>14)       
                    port map(x=>hjr,y=>hj2,clk=>pclk);         
                                                                  
  u40: pg_log_muldiv_1 generic map(PG_WIDTH=>14,
                                PG_MULDIV=>"ADD")
                    port map(x=>hjr,y=>hj2,z=>hj3,clk=>pclk);
                                                                  
  u41: pg_pdelay_1 generic map(PG_WIDTH=>14)  
                    port map(x=>hj2,y=>hj2r,clk=>pclk); 
                                                          
  u42: pg_log_muldiv_1 generic map(PG_WIDTH=>14,
                                PG_MULDIV=>"ADD")
                    port map(x=>hj3,y=>hj2r,z=>hj5,clk=>pclk);
                                                                  
  u43: pg_pdelay_1 generic map(PG_WIDTH=>14)  
                    port map(x=>hj3,y=>hj3r,clk=>pclk); 
                                                          
  u44: pg_log_muldiv_1 generic map(PG_WIDTH=>14,
                                PG_MULDIV=>"SUB")
                    port map(x=>r2,y=>hj2,z=>rhj2,clk=>pclk);
                                                                  
  u45: pg_wtable_14_5   port map (indata=>rhj2,outdata=>wj,clk=>pclk);                  
                                                                     
  u46: pg_log_muldiv_1 generic map(PG_WIDTH=>14,
                                PG_MULDIV=>"SUB")
                    port map(x=>wj,y=>hj3r,z=>whj,clk=>pclk);
                                                                  
  u47: pg_dwtable_14_5   port map (indata=>rhj2,outdata=>dwj,clk=>pclk);                  
                                                                     
  u48: pg_log_muldiv_1 generic map(PG_WIDTH=>14,
                                PG_MULDIV=>"SUB")
                    port map(x=>dwj,y=>hj5,z=>dwhj,clk=>pclk);
                                                                  
  u49: pg_log_unsigned_add_14_5      
            port map(x=>whi,y=>whj,z=>w,clock=>pclk);       
                                                                   
  u50: pg_log_unsigned_add_14_5      
            port map(x=>dwhi,y=>dwhj,z=>dw,clock=>pclk);       
                                                                   
  u51: pg_log_smuldiv_1_14 generic map(PG_WIDTH=>14,
                                PG_MULDIV=>"SUB")
                    port map(x=>w,y=>rhoshift,z=>wrhos,clk=>pclk);
                                                                  
  u52: pg_conv_ltof_14_5_57
                  port map (logdata=>wrhos,fixdata=>frho,clk=>pclk);
                                                                   
  u53: pg_fix_accum_57_64_2
                   port map(fdata=>frho,sdata0=>srho0,sdata1=>srho1,sdata2=>srho2,run=>run(22),clk=>pclk);
                                                                    
  u54: pg_pdelay_5 generic map(PG_WIDTH=>14)  
                    port map(x=>vdotr,y=>vdotrr,clk=>pclk); 
                                                          
  u55: pg_log_muldiv_1 generic map(PG_WIDTH=>14,
                                PG_MULDIV=>"ADD")
                    port map(x=>vdotrr,y=>dw,z=>vrdw0,clk=>pclk);
                                                                  
  u56: pg_log_smuldiv_1_14 generic map(PG_WIDTH=>14,
                                PG_MULDIV=>"SUB")
                    port map(x=>vrdw0,y=>dshift,z=>vrdw,clk=>pclk);
                                                                  
  u57: pg_conv_ltof_14_5_57
                  port map (logdata=>vrdw,fixdata=>fdivv,clk=>pclk);
                                                                   
  u58: pg_fix_accum_57_64_2
                   port map(fdata=>fdivv,sdata0=>sdivv0,sdata1=>sdivv1,sdata2=>sdivv2,run=>run(23),clk=>pclk);
                                                                    
  u59: pg_pdelay_13 generic map(PG_WIDTH=>14)  
                    port map(x=>pj,y=>pjr,clk=>pclk); 
                                                          
  u60: pg_pdelay_13 generic map(PG_WIDTH=>14)  
                    port map(x=>pi,y=>pir,clk=>pclk); 
                                                          
  u61: pg_log_muldiv_1 generic map(PG_WIDTH=>14,
                                PG_MULDIV=>"ADD")
                    port map(x=>pir,y=>pjr,z=>pij,clk=>pclk);
                                                                  
  u62: pg_pdelay_11 generic map(PG_WIDTH=>14)  
                    port map(x=>qj,y=>qjr,clk=>pclk); 
                                                          
  u63: pg_pdelay_11 generic map(PG_WIDTH=>14)  
                    port map(x=>qi,y=>qir,clk=>pclk); 
                                                          
  u64: pg_log_unsigned_add_14_5      
            port map(x=>qir,y=>qjr,z=>qij,clock=>pclk);       
                                                                   
  u65: pg_log_unsigned_add_14_5      
            port map(x=>pij,y=>qij,z=>avij,clock=>pclk);       
                                                                   
  u66: pg_log_muldiv_1 generic map(PG_WIDTH=>14,
                                PG_MULDIV=>"ADD")
                    port map(x=>dw,y=>avij,z=>avdw0,clk=>pclk);
                                                                  
  u67: pg_log_smuldiv_1_14 generic map(PG_WIDTH=>14,
                                PG_MULDIV=>"SUB")
                    port map(x=>avdw0,y=>fshift,z=>avdw,clk=>pclk);
                                                                  
  u68: pg_log_muldiv_1 generic map(PG_WIDTH=>14,
                                PG_MULDIV=>"ADD")
                    port map(x=>dxr,y=>avdw,z=>avdwx,clk=>pclk);
                                                                  
  u69: pg_log_muldiv_1 generic map(PG_WIDTH=>14,
                                PG_MULDIV=>"ADD")
                    port map(x=>dyr,y=>avdw,z=>avdwy,clk=>pclk);
                                                                  
  u70: pg_log_muldiv_1 generic map(PG_WIDTH=>14,
                                PG_MULDIV=>"ADD")
                    port map(x=>dzr,y=>avdw,z=>avdwz,clk=>pclk);
                                                                  
  u71: pg_conv_ltof_14_5_57
                  port map (logdata=>avdwx,fixdata=>ax,clk=>pclk);
                                                                   
  u72: pg_conv_ltof_14_5_57
                  port map (logdata=>avdwy,fixdata=>ay,clk=>pclk);
                                                                   
  u73: pg_conv_ltof_14_5_57
                  port map (logdata=>avdwz,fixdata=>az,clk=>pclk);
                                                                   
  u74: pg_fix_accum_57_64_2
                   port map(fdata=>ax,sdata0=>sax0,sdata1=>sax1,sdata2=>sax2,run=>run(24),clk=>pclk);
                                                                    
  u75: pg_fix_accum_57_64_2
                   port map(fdata=>ay,sdata0=>say0,sdata1=>say1,sdata2=>say2,run=>run(24),clk=>pclk);
                                                                    
  u76: pg_fix_accum_57_64_2
                   port map(fdata=>az,sdata0=>saz0,sdata1=>saz1,sdata2=>saz2,run=>run(24),clk=>pclk);
                                                                    
  u77: pg_log_smuldiv_1_14 generic map(PG_WIDTH=>14,
                                PG_MULDIV=>"SUB")
                    port map(x=>avdw0,y=>ushift,z=>avdw1,clk=>pclk);
                                                                  
  u78: pg_pdelay_2 generic map(PG_WIDTH=>14)  
                    port map(x=>vdotrr,y=>vdotrrr,clk=>pclk); 
                                                          
  u79: pg_log_muldiv_1 generic map(PG_WIDTH=>14,
                                PG_MULDIV=>"ADD")
                    port map(x=>vdotrrr,y=>avdw1,z=>dudt0,clk=>pclk);
                                                                  
  u80: pg_conv_ltof_14_5_57
                  port map (logdata=>dudt0,fixdata=>fdudt,clk=>pclk);
                                                                   
  u81: pg_fix_accum_57_64_2
                   port map(fdata=>fdudt,sdata0=>sdudt0,sdata1=>sdudt1,sdata2=>sdudt2,run=>run(24),clk=>pclk);
                                                                    
  process(pclk) begin
    if(pclk'event and pclk='1') then
      if(p_adro = "0000") then
        if(p_adrovp = "0000") then
          p_datao <= srho0(31 downto 0);
        elsif(p_adrovp = "0001") then
          p_datao <= srho1(31 downto 0);
        elsif(p_adrovp = "0010") then
          p_datao <= srho2(31 downto 0);
        end if;
      elsif(p_adro = "0001") then
        if(p_adrovp = "0000") then
          p_datao <= srho0(63 downto 32);
        elsif(p_adrovp = "0001") then
          p_datao <= srho1(63 downto 32);
        elsif(p_adrovp = "0010") then
          p_datao <= srho2(63 downto 32);
        end if;
      elsif(p_adro = "0010") then
        if(p_adrovp = "0000") then
          p_datao <= sdivv0(31 downto 0);
        elsif(p_adrovp = "0001") then
          p_datao <= sdivv1(31 downto 0);
        elsif(p_adrovp = "0010") then
          p_datao <= sdivv2(31 downto 0);
        end if;
      elsif(p_adro = "0011") then
        if(p_adrovp = "0000") then
          p_datao <= sdivv0(63 downto 32);
        elsif(p_adrovp = "0001") then
          p_datao <= sdivv1(63 downto 32);
        elsif(p_adrovp = "0010") then
          p_datao <= sdivv2(63 downto 32);
        end if;
      elsif(p_adro = "0100") then
        if(p_adrovp = "0000") then
          p_datao <= sax0(31 downto 0);
        elsif(p_adrovp = "0001") then
          p_datao <= sax1(31 downto 0);
        elsif(p_adrovp = "0010") then
          p_datao <= sax2(31 downto 0);
        end if;
      elsif(p_adro = "0101") then
        if(p_adrovp = "0000") then
          p_datao <= sax0(63 downto 32);
        elsif(p_adrovp = "0001") then
          p_datao <= sax1(63 downto 32);
        elsif(p_adrovp = "0010") then
          p_datao <= sax2(63 downto 32);
        end if;
      elsif(p_adro = "0110") then
        if(p_adrovp = "0000") then
          p_datao <= say0(31 downto 0);
        elsif(p_adrovp = "0001") then
          p_datao <= say1(31 downto 0);
        elsif(p_adrovp = "0010") then
          p_datao <= say2(31 downto 0);
        end if;
      elsif(p_adro = "0111") then
        if(p_adrovp = "0000") then
          p_datao <= say0(63 downto 32);
        elsif(p_adrovp = "0001") then
          p_datao <= say1(63 downto 32);
        elsif(p_adrovp = "0010") then
          p_datao <= say2(63 downto 32);
        end if;
      elsif(p_adro = "1000") then
        if(p_adrovp = "0000") then
          p_datao <= saz0(31 downto 0);
        elsif(p_adrovp = "0001") then
          p_datao <= saz1(31 downto 0);
        elsif(p_adrovp = "0010") then
          p_datao <= saz2(31 downto 0);
        end if;
      elsif(p_adro = "1001") then
        if(p_adrovp = "0000") then
          p_datao <= saz0(63 downto 32);
        elsif(p_adrovp = "0001") then
          p_datao <= saz1(63 downto 32);
        elsif(p_adrovp = "0010") then
          p_datao <= saz2(63 downto 32);
        end if;
      elsif(p_adro = "1010") then
        if(p_adrovp = "0000") then
          p_datao <= sdudt0(31 downto 0);
        elsif(p_adrovp = "0001") then
          p_datao <= sdudt1(31 downto 0);
        elsif(p_adrovp = "0010") then
          p_datao <= sdudt2(31 downto 0);
        end if;
      elsif(p_adro = "1011") then
        if(p_adrovp = "0000") then
          p_datao <= sdudt0(63 downto 32);
        elsif(p_adrovp = "0001") then
          p_datao <= sdudt1(63 downto 32);
        elsif(p_adrovp = "0010") then
          p_datao <= sdudt2(63 downto 32);
        end if;
      else
        p_datao <= "00000000000000000000000000000000";
      end if;
    end if;
  end process;

end std;                                                            
                                                                    
