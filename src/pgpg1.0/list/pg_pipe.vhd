library ieee;                                                  
use ieee.std_logic_1164.all;                                   
                                                               
entity pg_pipe is                                              
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
end pg_pipe;                                                   
                                                               
architecture std of pg_pipe is                                 
                                                               
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
  signal we,runret: std_logic_vector(1 downto 0);
  signal datao: std_logic_vector(63 downto 0);
  signal l_adro: std_logic_vector(3 downto 0);                 
                                                               
begin                                                          
                                                               
  u_adri <= p_adri(11 downto 4);                               
                                                               
  u_adro <= p_adro(11 downto 4);                               
  l_adro <= p_adro(3 downto 0);                                
                                                               
  process(u_adri,p_we) begin                                 
    if(p_we = '1') then                                      
     if(u_adri = "00000000" or u_adri = "00000001" ) then
        we(0) <= '1';                                    
      else                                                   
        we(0) <= '0';                                    
      end if;                                                
    else                                                     
      we(0) <= '0';                                      
    end if;                                                  
  end process;                                               
                                                             
  process(u_adri,p_we) begin                                 
    if(p_we = '1') then                                      
     if(u_adri = "00000010" or u_adri = "00000011" ) then
        we(1) <= '1';                                    
      else                                                   
        we(1) <= '0';                                    
      end if;                                                
    else                                                     
      we(1) <= '0';                                      
    end if;                                                  
  end process;                                               
                                                             
  with u_adri select                                           
    adrivp <= "0000" when "00000000", 
              "0001" when "00000001", 
              "0000" when "00000010", 
              "0001" when "00000011", 
              "0000" when others;                            
                                                               
  with u_adro select                                           
    adrovp <= "0000" when "00000000", 
              "0001" when "00000001", 
              "0000" when "00000010", 
              "0001" when "00000011", 
              "0000" when others;                            
                                                               
  forgen1: for i in 0 to 1 generate           
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
               datao(63 downto 32) when "00000010", 
               datao(63 downto 32) when "00000011", 
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
                                                                   
  component pg_conv_ftol_32_17_8                       
    port(fixdata : in std_logic_vector(31 downto 0);     
         logdata : out std_logic_vector(16 downto 0);    
         clk : in std_logic);                                      
  end component;                                                   
                                                                   
  component pg_pdelay_8                            
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
                                                           
  component pg_pdelay_5                            
    generic (PG_WIDTH: integer);                          
    port( x : in std_logic_vector(PG_WIDTH-1 downto 0);   
	     y : out std_logic_vector(PG_WIDTH-1 downto 0); 
	     clk: in std_logic);                            
  end component;                                          
                                                          
  component pg_log_unsigned_add_17_8                    
    port( x,y : in std_logic_vector(16 downto 0);      
          z : out std_logic_vector(16 downto 0);       
          clock : in std_logic);                                   
  end component;                                                   
                                                                   
  component pg_log_shift_m1                        
    generic (PG_WIDTH: integer);                           
    port( x : in std_logic_vector(PG_WIDTH-1 downto 0);    
	     y : out std_logic_vector(PG_WIDTH-1 downto 0);  
	     clk: in std_logic);                             
  end component;                                           
                                                           
  component pg_log_muldiv_1                             
    generic (PG_WIDTH : integer;                                 
             PG_MULDIV : string);                                
    port( x,y : in std_logic_vector(PG_WIDTH-1 downto 0);        
	     z : out std_logic_vector(PG_WIDTH-1 downto 0);        
	     clk: in std_logic);                                   
  end component;                                                 
                                                                 
  component pg_pdelay_12                            
    generic (PG_WIDTH: integer);                          
    port( x : in std_logic_vector(PG_WIDTH-1 downto 0);   
	     y : out std_logic_vector(PG_WIDTH-1 downto 0); 
	     clk: in std_logic);                            
  end component;                                          
                                                          
  component pg_conv_ltof_17_8_57      
    port(logdata : in std_logic_vector(16 downto 0);   
	     fixdata : out std_logic_vector(56 downto 0);  
	     clk : in std_logic);                                     
  end component;                                                  
                                                                  
  component pg_fix_accum_57_64_2                          
    port (fdata: in std_logic_vector(56 downto 0);    
          sdata0: out std_logic_vector(63 downto 0);   
          sdata1: out std_logic_vector(63 downto 0);   
          run: in std_logic;                                        
          clk: in std_logic);                                       
  end component;                                                    
                                                                    
  signal run: std_logic_vector(18 downto 0);                        
  signal xi: std_logic_vector(31 downto 0);    
  signal xj: std_logic_vector(31 downto 0);    
  signal yi: std_logic_vector(31 downto 0);    
  signal yj: std_logic_vector(31 downto 0);    
  signal zi: std_logic_vector(31 downto 0);    
  signal zj: std_logic_vector(31 downto 0);    
  signal xij: std_logic_vector(31 downto 0);    
  signal yij: std_logic_vector(31 downto 0);    
  signal zij: std_logic_vector(31 downto 0);    
  signal dx: std_logic_vector(16 downto 0);    
  signal dy: std_logic_vector(16 downto 0);    
  signal dz: std_logic_vector(16 downto 0);    
  signal ieps2: std_logic_vector(16 downto 0);    
  signal x2: std_logic_vector(16 downto 0);           
  signal y2: std_logic_vector(16 downto 0);           
  signal z2: std_logic_vector(16 downto 0);           
  signal ieps2r: std_logic_vector(16 downto 0);           
  signal x2y2: std_logic_vector(16 downto 0);           
  signal z2e2: std_logic_vector(16 downto 0);           
  signal r2: std_logic_vector(16 downto 0);    
  signal r1: std_logic_vector(16 downto 0);    
  signal mj: std_logic_vector(16 downto 0);    
  signal mjr: std_logic_vector(16 downto 0);    
  signal r3: std_logic_vector(16 downto 0);    
  signal mf: std_logic_vector(16 downto 0);    
  signal dxr: std_logic_vector(16 downto 0);    
  signal dyr: std_logic_vector(16 downto 0);    
  signal dzr: std_logic_vector(16 downto 0);    
  signal fx: std_logic_vector(16 downto 0);    
  signal fy: std_logic_vector(16 downto 0);    
  signal fz: std_logic_vector(16 downto 0);    
  signal ffx: std_logic_vector(56 downto 0);    
  signal sx0: std_logic_vector(63 downto 0);    
  signal sx1: std_logic_vector(63 downto 0);    
  signal ffy: std_logic_vector(56 downto 0);    
  signal sy0: std_logic_vector(63 downto 0);    
  signal sy1: std_logic_vector(63 downto 0);    
  signal ffz: std_logic_vector(56 downto 0);    
  signal sz0: std_logic_vector(63 downto 0);    
  signal sz1: std_logic_vector(63 downto 0);    
  signal xi0: std_logic_vector(31 downto 0);
  signal xi1: std_logic_vector(31 downto 0);
  signal yi0: std_logic_vector(31 downto 0);
  signal yi1: std_logic_vector(31 downto 0);
  signal zi0: std_logic_vector(31 downto 0);
  signal zi1: std_logic_vector(31 downto 0);
  signal ieps20: std_logic_vector(16 downto 0);
  signal ieps21: std_logic_vector(16 downto 0);
  signal vmp_phase: std_logic_vector(3 downto 0);                 
  signal irun: std_logic_vector(1 downto 0);      
  signal jdata1: std_logic_vector(JDATA_WIDTH-1 downto 0);      
                                                                    
begin                                                               
                                                                    
  process(pclk) begin
    if(pclk'event and pclk='1') then
      jdata1 <= p_jdata;            
    end if;
  end process;
              
  process(pclk) begin
    if(pclk'event and pclk='1') then
      if(vmp_phase = "0000") then
        xj(31 downto 0) <= jdata1(31 downto 0);
        yj(31 downto 0) <= jdata1(63 downto 32);
        zj(31 downto 0) <= p_jdata(31 downto 0);
        mj(16 downto 0) <= p_jdata(48 downto 32);
      end if;
    end if;
  end process;

  process(pclk) begin
    if(pclk'event and pclk='1') then
      if(p_we ='1') then
        if(p_adri = "0000") then
          if(p_adrivp = "0000") then
            xi0 <=  p_datai(31 downto 0);
          elsif(p_adrivp = "0001") then
            xi1 <=  p_datai(31 downto 0);
          end if;
        elsif(p_adri = "0001") then
          if(p_adrivp = "0000") then
            yi0 <=  p_datai(31 downto 0);
          elsif(p_adrivp = "0001") then
            yi1 <=  p_datai(31 downto 0);
          end if;
        elsif(p_adri = "0010") then
          if(p_adrivp = "0000") then
            zi0 <=  p_datai(31 downto 0);
          elsif(p_adrivp = "0001") then
            zi1 <=  p_datai(31 downto 0);
          end if;
        elsif(p_adri = "0011") then
          if(p_adrivp = "0000") then
            ieps20 <=  p_datai(16 downto 0);
          elsif(p_adrivp = "0001") then
            ieps21 <=  p_datai(16 downto 0);
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
        ieps2 <= ieps20;
      elsif(vmp_phase = "0001") then
        xi <= xi1;
        yi <= yi1;
        zi <= zi1;
        ieps2 <= ieps21;
      end if;
    end if;
  end process;

  process(pclk) begin                                 
    if(pclk'event and pclk='1') then                  
      irun(0) <= p_run;                               
      irun(1) <= irun(0);                   
    end if;                                           
  end process;                                        
                                                      
  process(pclk) begin                                 
    if(pclk'event and pclk='1') then                  
      if(irun(1) = '1') then           
        if(vmp_phase = "0001") then              
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
      for i in 0 to 17 loop                                         
        run(i+1) <= run(i);                                         
      end loop;                                                     
      p_runret <= run(18);                                          
    end if;                                                         
  end process;                                                      
                                                                    
  u0: pg_fix_addsub generic map (PG_WIDTH=>32,PG_PIPELINE=>1,     
                              PG_DIRECTION=>"SUB")                
               port map (x=>xi,y=>xj,z=>xij,clk=>pclk);             
                                                                   
  u1: pg_fix_addsub generic map (PG_WIDTH=>32,PG_PIPELINE=>1,     
                              PG_DIRECTION=>"SUB")                
               port map (x=>yi,y=>yj,z=>yij,clk=>pclk);             
                                                                   
  u2: pg_fix_addsub generic map (PG_WIDTH=>32,PG_PIPELINE=>1,     
                              PG_DIRECTION=>"SUB")                
               port map (x=>zi,y=>zj,z=>zij,clk=>pclk);             
                                                                   
  u3: pg_conv_ftol_32_17_8   port map (fixdata=>xij,logdata=>dx,clk=>pclk);                 
                                                                     
  u4: pg_conv_ftol_32_17_8   port map (fixdata=>yij,logdata=>dy,clk=>pclk);                 
                                                                     
  u5: pg_conv_ftol_32_17_8   port map (fixdata=>zij,logdata=>dz,clk=>pclk);                 
                                                                     
  u6: pg_pdelay_8 generic map(PG_WIDTH=>17)  
                    port map(x=>dx,y=>dxr,clk=>pclk); 
                                                          
  u7: pg_pdelay_8 generic map(PG_WIDTH=>17)  
                    port map(x=>dy,y=>dyr,clk=>pclk); 
                                                          
  u8: pg_pdelay_8 generic map(PG_WIDTH=>17)  
                    port map(x=>dz,y=>dzr,clk=>pclk); 
                                                          
  u9: pg_log_shift_1 generic map(PG_WIDTH=>17)       
                    port map(x=>dx,y=>x2,clk=>pclk);         
                                                                  
  u10: pg_log_shift_1 generic map(PG_WIDTH=>17)       
                    port map(x=>dy,y=>y2,clk=>pclk);         
                                                                  
  u11: pg_log_shift_1 generic map(PG_WIDTH=>17)       
                    port map(x=>dz,y=>z2,clk=>pclk);         
                                                                  
  u12: pg_pdelay_5 generic map(PG_WIDTH=>17)  
                    port map(x=>ieps2,y=>ieps2r,clk=>pclk); 
                                                          
  u13: pg_log_unsigned_add_17_8      
            port map(x=>x2,y=>y2,z=>x2y2,clock=>pclk);       
                                                                   
  u14: pg_log_unsigned_add_17_8      
            port map(x=>z2,y=>ieps2r,z=>z2e2,clock=>pclk);       
                                                                   
  u15: pg_log_unsigned_add_17_8      
            port map(x=>x2y2,y=>z2e2,z=>r2,clock=>pclk);       
                                                                   
  u16: pg_log_shift_m1 generic map(PG_WIDTH=>17)       
                    port map(x=>r2,y=>r1,clk=>pclk);         
                                                                  
  u17: pg_log_muldiv_1 generic map(PG_WIDTH=>17,
                                PG_MULDIV=>"ADD")
                    port map(x=>r2,y=>r1,z=>r3,clk=>pclk);
                                                                  
  u18: pg_pdelay_12 generic map(PG_WIDTH=>17)  
                    port map(x=>mj,y=>mjr,clk=>pclk); 
                                                          
  u19: pg_log_muldiv_1 generic map(PG_WIDTH=>17,
                                PG_MULDIV=>"SUB")
                    port map(x=>mjr,y=>r3,z=>mf,clk=>pclk);
                                                                  
  u20: pg_log_muldiv_1 generic map(PG_WIDTH=>17,
                                PG_MULDIV=>"ADD")
                    port map(x=>mf,y=>dxr,z=>fx,clk=>pclk);
                                                                  
  u21: pg_log_muldiv_1 generic map(PG_WIDTH=>17,
                                PG_MULDIV=>"ADD")
                    port map(x=>mf,y=>dyr,z=>fy,clk=>pclk);
                                                                  
  u22: pg_log_muldiv_1 generic map(PG_WIDTH=>17,
                                PG_MULDIV=>"ADD")
                    port map(x=>mf,y=>dzr,z=>fz,clk=>pclk);
                                                                  
  u23: pg_conv_ltof_17_8_57
                  port map (logdata=>fx,fixdata=>ffx,clk=>pclk);
                                                                   
  u24: pg_conv_ltof_17_8_57
                  port map (logdata=>fy,fixdata=>ffy,clk=>pclk);
                                                                   
  u25: pg_conv_ltof_17_8_57
                  port map (logdata=>fz,fixdata=>ffz,clk=>pclk);
                                                                   
  u26: pg_fix_accum_57_64_2
                   port map(fdata=>ffx,sdata0=>sx0,sdata1=>sx1,run=>run(17),clk=>pclk);
                                                                    
  u27: pg_fix_accum_57_64_2
                   port map(fdata=>ffy,sdata0=>sy0,sdata1=>sy1,run=>run(17),clk=>pclk);
                                                                    
  u28: pg_fix_accum_57_64_2
                   port map(fdata=>ffz,sdata0=>sz0,sdata1=>sz1,run=>run(17),clk=>pclk);
                                                                    
  process(pclk) begin
    if(pclk'event and pclk='1') then
      if(p_adro = "0000") then
        if(p_adrovp = "0000") then
          p_datao <= sx0(31 downto 0);
        elsif(p_adrovp = "0001") then
          p_datao <= sx1(31 downto 0);
        end if;
      elsif(p_adro = "0001") then
        if(p_adrovp = "0000") then
          p_datao <= sx0(63 downto 32);
        elsif(p_adrovp = "0001") then
          p_datao <= sx1(63 downto 32);
        end if;
      elsif(p_adro = "0010") then
        if(p_adrovp = "0000") then
          p_datao <= sy0(31 downto 0);
        elsif(p_adrovp = "0001") then
          p_datao <= sy1(31 downto 0);
        end if;
      elsif(p_adro = "0011") then
        if(p_adrovp = "0000") then
          p_datao <= sy0(63 downto 32);
        elsif(p_adrovp = "0001") then
          p_datao <= sy1(63 downto 32);
        end if;
      elsif(p_adro = "0100") then
        if(p_adrovp = "0000") then
          p_datao <= sz0(31 downto 0);
        elsif(p_adrovp = "0001") then
          p_datao <= sz1(31 downto 0);
        end if;
      elsif(p_adro = "0101") then
        if(p_adrovp = "0000") then
          p_datao <= sz0(63 downto 32);
        elsif(p_adrovp = "0001") then
          p_datao <= sz1(63 downto 32);
        end if;
      else
        p_datao <= "00000000000000000000000000000000";
      end if;
    end if;
  end process;

end std;                                                            
                                                                    
